#include "AlgorithmAStar.h"
#include <list>
#include <cstdlib>

std::string AStarStringFindStatus (AStarFindStatus status)
{
    switch (status)
    {
        case AStarFindStatus::BAD_MAP:         return "BAD_MAP";         break;
        case AStarFindStatus::OUT_OF_MAP:      return "OUT_OF_MAP";      break;
        case AStarFindStatus::NOT_ACCESSIBLE:  return "NOT_ACCESSIBLE";  break;
        case AStarFindStatus::START_EQUAL_END: return "START_EQUAL_END"; break;
        case AStarFindStatus::NOT_FOUND:       return "NOT_FOUND";       break;
        case AStarFindStatus::SUCCESS:         return "SUCCESS";         break;
        default: return "";
    }
}

// Задачи для оптимизации:
// 1. Из списка открытых точек выбрать точку с наименьшей оценкой. Решение: своя реализация очереди 
//            (std::priority_queue не позволяет изменять значения с пересортировкой). 
// 2. Для данной точки найти соседей. Решение: перебор элементов массива направлений. 
// 3. Найти, имеется ли точка в закрытом списке. Решение: статус в одномерном массиве. 
// 4. Найти, имеется ли точка в открытом списке. Решение: статус в одномерном массиве. 
// 5. В конце поиска найти путь от конца до начала. Решение: номера направлений в одномерном массиве. 

// Есть два варианта работы со списком открытых точек: использовать сортированный список, или не сортированный. 
// В первом случае нет поиска при нахождении точки с наименьшей оценкой, но есть поиск при добавлении точек,
// а так же при изменении оценки уже существующей в списке точки. Во втором случае все добавления и изменения 
// без поиска, а поиск только один - при нахождении точки с наименьшей оценкой - но зато по всем элементам.
// Однако учтем, что когда мы находим точку из открытого списка и работаем с ее соседями, полная оценка пути 
// для этих точек будет различаться не намного. Таким образом, если работать с сортированным списком, во время 
// каждого поиска будет пройдено совсем мало элементов с начала списка. И в сумме будет намного меньше операций, 
// чем при однократном полном поиске по всем элементам. 

// Был выбран контейнер forward_list, так как произвольный доступ к элементам не нужен - будет только перебор с начала. 
// Зато будут вставки в середину контейнера и удаление оттуда, а так же перемещение элементов внутри списка. 
// Но в поиске длинных путей, когда обрабатывается около четверти всех точек карты, время поиска было более 10 секунд. 
// Профилирование показало, что треть всего выполнения программы уходит на операцию ++ для итератора forward_list.
// Отчасти это вызвано необходимостью вести два итератора - на текущий элемент и на элемент перед ним. 
// Для уменьшения количества операций с итераторами контейнер изменен на list. 
// Хотя количество операций с итераторами снизилось в 2 раза, ситуация по времени улучшилась не намного. 
// Решено для списка открытых точек использовать бинарную кучу. Была попытка использовать контейнер vector и 
// стандартные функции - make_heap, push_heap, pop_heap. Но время работы даже увеличилось, а количество вызовов 
// операции ++ для итераторов возросло более чем в 2 раза по сравнению с контейнером list. 
// Видимо, лучше будет реализовать кучу своим кодом. 
// (судя по Википедии, самая быстрая куча - очередь Бродала, хотя для начала попробую что-нибудь попроще)

// Еще возможные оптимизации: контейнер с быстрым произвольным доступом и использование бинарного поиска; 
// можно поработать с битовой картой - например, брать только один пиксель из квадрата 2*2 или 3*3; 
// можно так же разделить карту на прямоугольные области с точками входа и выхода, и искать путь 
// только внутри текущей области.
// Однако, лучшим вариантом видится реализация улучшенного алгоритма A* - Jump Point Search. 

// Реализация A* с вручную написанной min-кучей находится в файле AlgorithmAStar_v2.cpp. 

struct AStarNode {int x, y, full_patch_cost;};

class AStarOrderedList 
{
public:
    std::list <AStarNode *> list;
    
    void add (AStarNode * val)
    {
        if (list.empty())
        {
            list.push_front(val);
        }
        else {
            auto current = list.begin();
            auto end = list.end();
            bool inserted = false;
            while (current != end)
            {
                if ((*current)->full_patch_cost >= val->full_patch_cost)
                {
                    list.insert(current, val);
                    inserted = true;
                    break;
                }
                ++current;
            }
            if (!inserted) list.push_back(val);
        }
    }
    
    void resort_element (AStarNode * val, int delta_patch_cost)
    {
        auto current = list.begin();
        auto end = list.end();
        while (current != end)
        {
            if (*current == val)
            {
                val->full_patch_cost += delta_patch_cost;
                list.erase(current);
                add(val);
                break;
            }
            ++current;
        }
    }
    
    void pop_front ()
    {
        delete list.front();
        list.pop_front();
    }
};

// Массив направлений - всего 8 направлений.
static const struct {signed char dx, dy, cost;} AStarDirections [8] = {
    {1, 0, 10}, {1, 1, 14}, {0, 1, 10}, {-1, 1, 14},
    {-1, 0, 10}, {-1, -1, 14}, {0, -1, 10}, {1, -1, 14}
};

// Индекс - номер направления, значение - номер противоположного направления.
static const signed char AStarOppositeDirection [8] = {4, 5, 6, 7, 0, 1, 2, 3};

enum AStarPointStatus {NONE = 0, OPENED = 1, CLOSED = 2};

struct AStarNodeInfo
{
    unsigned char status;            // Одно из значений AStarPointStatus. 
    signed char direction_to_parent; // Номер направления на родительскую точку. 
    int from_begin_cost;             // Цена пути от начала до текущей точки.
    AStarNode * pointer;             // Указатель на точку открытого списка для быстрого поиска. 
};

inline int AStarGetEvaluation (int x1, int y1, int x2, int y2)
{
    return (abs(x2 - x1) + abs(y2 - y1)) * 10;
}

// Открытый список не должен быть пустой.
// Если в открытый список добавляется финишная точка, возвращается true, иначе false.
bool AStarStep (
    AStarOrderedList & open_list, 
    const std::vector <std::vector <bool>> & bit_map,
    int bit_map_width,
    int bit_map_height,
    int end_x,
    int end_y,
    AStarNodeInfo * nodes_info
) {
    int current_x = (*open_list.list.front()).x;
    int current_y = (*open_list.list.front()).y;
    int current_shift = current_y * bit_map_width + current_x;
    open_list.pop_front();
    //nodes_info[current_shift].pointer = nullptr; // можно не менять
    nodes_info[current_shift].status = AStarPointStatus::CLOSED;
    
    for (int i = 0; i < 8; ++i)
    {
        int nx = current_x + AStarDirections[i].dx;
        int ny = current_y + AStarDirections[i].dy;
        int n_shift = ny * bit_map_width + nx;
        if (
            nx >= 0 && nx < bit_map_width && ny >= 0 && ny < bit_map_height
            && !bit_map[nx][ny] && nodes_info[n_shift].status != AStarPointStatus::CLOSED
            
        ) {
            int n_from_begin_cost = nodes_info[current_shift].from_begin_cost + AStarDirections[i].cost;
            if (nodes_info[n_shift].status == AStarPointStatus::OPENED)
            {
                if (n_from_begin_cost < nodes_info[n_shift].from_begin_cost)
                {
                    int delta_cost = n_from_begin_cost - nodes_info[n_shift].from_begin_cost;
                    nodes_info[n_shift].direction_to_parent = AStarOppositeDirection[i];
                    nodes_info[n_shift].from_begin_cost = n_from_begin_cost;
                    open_list.resort_element(nodes_info[n_shift].pointer, delta_cost);
                }
            }
            else {
                nodes_info[n_shift].status = AStarPointStatus::OPENED;
                nodes_info[n_shift].direction_to_parent = AStarOppositeDirection[i];
                nodes_info[n_shift].from_begin_cost = n_from_begin_cost;
                AStarNode * point = new AStarNode {nx, ny, n_from_begin_cost + AStarGetEvaluation(nx, ny, end_x, end_y)};
                open_list.add(point);
                nodes_info[n_shift].pointer = point;
                
                if (nx == end_x && ny == end_y) return true;
            }
        }
    }
    
    return false;
}

AStarFindResults AStarFindPatch (
    const std::vector <std::vector <bool>> & bit_map, 
    int start_x, int start_y,
    int end_x, int end_y
) {
    int width = bit_map.size();
    int height = (width > 0 ? bit_map[0].size() : 0);
    
    if (width == 0 || height == 0)
        return AStarFindResults{AStarFindStatus::BAD_MAP, {}};
    
    if (
        start_x < 0 || start_x >= width || start_y < 0 || start_y >= height 
        || end_x < 0 || end_x >= width || end_y < 0 || end_y >= height 
    )
        return AStarFindResults{AStarFindStatus::OUT_OF_MAP, {}};
    
    if (bit_map[start_x][start_y] || bit_map[end_x][end_y])
        return AStarFindResults{AStarFindStatus::NOT_ACCESSIBLE, {}};
    
    if (start_x == end_x && start_y == end_y)
        return AStarFindResults{AStarFindStatus::START_EQUAL_END, {}};
    
    int pixels_count = width * height;
    // Массив с данными о точках. Точка x, y находится по смещению [y * width + x]. 
    AStarNodeInfo * nodes_info = new AStarNodeInfo [pixels_count];
    // Значения по умолчанию. 
    for (int i = 0; i < pixels_count; ++i)
    {
        nodes_info[i].status = AStarPointStatus::NONE;
        nodes_info[i].direction_to_parent = -1;
        nodes_info[i].from_begin_cost = -1;
        nodes_info[i].pointer = nullptr;
    }
    
    AStarOrderedList open_list;
    bool patch_found = false;
    AStarFindResults result {AStarFindStatus::NOT_FOUND, {}};
    
    AStarNode * start_point = new AStarNode {start_x, start_y, AStarGetEvaluation(start_x, start_y, end_x, end_y)};
    open_list.add(start_point);
    int pointer_shift = start_y * width + start_x;
    nodes_info[pointer_shift].pointer = start_point;
    nodes_info[pointer_shift].status = AStarPointStatus::OPENED;
    nodes_info[pointer_shift].from_begin_cost = 0;
    
    while (!open_list.list.empty() && !patch_found)
        patch_found = AStarStep(open_list, bit_map, width, height, end_x, end_y, nodes_info);
    
    if (patch_found)
    {
        result.status = AStarFindStatus::SUCCESS;
        int current_x = end_x;
        int current_y = end_y;
        // В начальной точке direction_to_parent равно -1 
        while (current_x != start_x || current_y != start_y)
        {
            result.patch.emplace_front <std::pair <int, int>> ({current_x, current_y});
            auto direction_number = nodes_info[current_y * width + current_x].direction_to_parent;
            current_x += AStarDirections[direction_number].dx;
            current_y += AStarDirections[direction_number].dy;
        }
    }
    
    for (auto iter = open_list.list.begin(); iter != open_list.list.end(); ++iter) delete *iter;
    delete [] nodes_info;
    
    return result;
}
