#include "AlgorithmJPS_v2.h"
#include "BinaryHeap.h"
#include <thread>

// Сделана дополнительная оптимизация - запускать в отдельных потоках функцию JPSFindPoint. В ней не происходит изменений, 
// вроде добавления прыжковых точек в открытый список, а только читаются данные. Кроме того, она работает независимо 
// для каждого направления поиска, и результат работы в одном направлении не влияет на результаты работы в других 
// направлениях. Поэтому ее безопасно запускать в нескольких потоках. 

// Однако, по результатам измерений, этот вариант выполняется в 2.7 раза медленнее. Видимо, функция JPSFindPoint 
// работает достаточно быстро и требует меньше времени, чем нужно на выделение отдельного потока.

std::string JPSStringFindStatus (JPSFindStatus status)
{
    switch (status)
    {
        case JPSFindStatus::BAD_MAP:         return "BAD_MAP";         break;
        case JPSFindStatus::OUT_OF_MAP:      return "OUT_OF_MAP";      break;
        case JPSFindStatus::NOT_ACCESSIBLE:  return "NOT_ACCESSIBLE";  break;
        case JPSFindStatus::START_EQUAL_END: return "START_EQUAL_END"; break;
        case JPSFindStatus::NOT_FOUND:       return "NOT_FOUND";       break;
        case JPSFindStatus::SUCCESS:         return "SUCCESS";         break;
        default: return "";
    }
}

// Для работы с номерами направлений.
inline static signed char mod8 (signed char value)
{
    while (value < 0) value += 8;
    while (value > 7) value -= 8;
    return value;
}

// Данные, связанные с направлениями. 

// Дельта координат. 
static const struct {signed char dx, dy;} JPSDirectionDelta [8] = {
    {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
};
// Диагональный ли путь. 
//static const bool JPSDirectionIsDiagonal [8] = {false, true, false, true, false, true, false, true};
static const bool JPSDirectionIsDiagonal [8] = {
    JPSDirectionDelta[0].dx != 0 && JPSDirectionDelta[0].dy != 0,
    JPSDirectionDelta[1].dx != 0 && JPSDirectionDelta[1].dy != 0,
    JPSDirectionDelta[2].dx != 0 && JPSDirectionDelta[2].dy != 0,
    JPSDirectionDelta[3].dx != 0 && JPSDirectionDelta[3].dy != 0,
    JPSDirectionDelta[4].dx != 0 && JPSDirectionDelta[4].dy != 0,
    JPSDirectionDelta[5].dx != 0 && JPSDirectionDelta[5].dy != 0,
    JPSDirectionDelta[6].dx != 0 && JPSDirectionDelta[6].dy != 0,
    JPSDirectionDelta[7].dx != 0 && JPSDirectionDelta[7].dy != 0
};
// Цена пути по данному направлению. 
//static const signed char JPSDirectionCost [8] = {10, 14, 10, 14, 10, 14, 10, 14};
static const signed char JPSDirectionCost [8] = {
    static_cast <signed char> (JPSDirectionIsDiagonal[0] ? 14 : 10),
    static_cast <signed char> (JPSDirectionIsDiagonal[1] ? 14 : 10), 
    static_cast <signed char> (JPSDirectionIsDiagonal[2] ? 14 : 10), 
    static_cast <signed char> (JPSDirectionIsDiagonal[3] ? 14 : 10), 
    static_cast <signed char> (JPSDirectionIsDiagonal[4] ? 14 : 10), 
    static_cast <signed char> (JPSDirectionIsDiagonal[5] ? 14 : 10), 
    static_cast <signed char> (JPSDirectionIsDiagonal[6] ? 14 : 10), 
    static_cast <signed char> (JPSDirectionIsDiagonal[7] ? 14 : 10)
};
// Индекс - номер направления, значение - номер противоположного направления.
static const signed char JPSOppositeDirection [8] = {4, 5, 6, 7, 0, 1, 2, 3};
// Направления, которые необходимо проверить на препятствия, а так же направления для добавления соседей. 
static const struct {signed char check, neighbor;} JPSDirectionChecks1 [8] = {
    {mod8(0 + 2), mod8(0 + 1)}, {mod8(1 + 3), mod8(1 + 2)}, {mod8(2 + 2), mod8(2 + 1)}, {mod8(3 + 3), mod8(3 + 2)}, 
    {mod8(4 + 2), mod8(4 + 1)}, {mod8(5 + 3), mod8(5 + 2)}, {mod8(6 + 2), mod8(6 + 1)}, {mod8(7 + 3), mod8(7 + 2)}
};
static const struct {signed char check, neighbor;} JPSDirectionChecks2 [8] = {
    {mod8(0 - 2), mod8(0 - 1)}, {mod8(1 - 3), mod8(1 - 2)}, {mod8(2 - 2), mod8(2 - 1)}, {mod8(3 - 3), mod8(3 - 2)}, 
    {mod8(4 - 2), mod8(4 - 1)}, {mod8(5 - 3), mod8(5 - 2)}, {mod8(6 - 2), mod8(6 - 1)}, {mod8(7 - 3), mod8(7 - 2)}
};
// Дополнительные (вертикальные и горизонтальные) соседи при диагональном движении.
static const struct {signed char first, second;} JPSDirectionDiagNeighbors[8] = {
    {-1, -1}, {mod8(1 + 1), mod8(1 - 1)}, {-1, -1}, {mod8(3 + 1), mod8(3 - 1)}, 
    {-1, -1}, {mod8(5 + 1), mod8(5 - 1)}, {-1, -1}, {mod8(7 + 1), mod8(7 - 1)}
};

// NOTE Можно не проверять нахождение координат внутри карты, так как границы карты обозначены препятствиями. 
// Хотя функция быстрая, но вызов 10 миллионов раз за каждый поиск на карте 2000x2000, занимает четверть времени,
// которое тратится на вызовы функции JPSIsJumpPoint.
/*inline bool is_in_bitmap (int x, int y, int bit_map_width, int bit_map_height)
{
    return x >= 0 && x < bit_map_width && y >= 0 && y < bit_map_height;
}*/

// Получить оценку стоимости пути между точками. 
inline int JPSGetEvaluation (int x1, int y1, int x2, int y2)
{
    return (abs(x2 - x1) + abs(y2 - y1)) * 10;
}

struct JPSNode
{
    int x;
    int y;
    int from_begin_cost;               // Цена пути от начала до текущей точки.
    bool is_opened;                    // Если false, то точка в закрытом списке, иначе в открытом.
    signed char direction_from_parent; // Номер направления с родительской точки на эту. У стартовой точки -1. 
    int parent_x;
    int parent_y;
};

// Проверка первых двух правил, при диагональном движении нужно проверять дополнительно.
inline bool JPSIsJumpPoint (
    int x, 
    int y,
    signed char direction,
    bool * bool_bit_map,
    int bit_map_width,
    int bit_map_height,
    int end_x,
    int end_y
) {
    if (x == end_x && y == end_y) return true;
    
    auto check_direction = JPSDirectionChecks1[direction].check;
    int check_x = x + JPSDirectionDelta[check_direction].dx;
    int check_y = y + JPSDirectionDelta[check_direction].dy;
    if (
        /*is_in_bitmap(check_x, check_y, bit_map_width, bit_map_height) 
        &&*/ bool_bit_map[check_y * bit_map_width + check_x]
    ) {
        auto neighbor_direction = JPSDirectionChecks1[direction].neighbor;
        int neighbor_x = x + JPSDirectionDelta[neighbor_direction].dx;
        int neighbor_y = y + JPSDirectionDelta[neighbor_direction].dy;
        if (
            /*is_in_bitmap(neighbor_x, neighbor_y, bit_map_width, bit_map_height)
            &&*/ !bool_bit_map[neighbor_y * bit_map_width + neighbor_x]
        ) 
            return true;
    }
    
    check_direction = JPSDirectionChecks2[direction].check;
    check_x = x + JPSDirectionDelta[check_direction].dx;
    check_y = y + JPSDirectionDelta[check_direction].dy;
    if (
        /*is_in_bitmap(check_x, check_y, bit_map_width, bit_map_height) 
        &&*/ bool_bit_map[check_y * bit_map_width + check_x]
    ) {
        auto neighbor_direction = JPSDirectionChecks2[direction].neighbor;
        int neighbor_x = x + JPSDirectionDelta[neighbor_direction].dx;
        int neighbor_y = y + JPSDirectionDelta[neighbor_direction].dy;
        if (
            /*is_in_bitmap(neighbor_x, neighbor_y, bit_map_width, bit_map_height)
            &&*/ !bool_bit_map[neighbor_y * bit_map_width + neighbor_x]
        )
            return true;
    }
    
    return false;
}

// На обработку передается текущая прыжковая точка и направление с нее.
// В result записывается JPSNode с найденной следующей прыжковой точкой, либо nullptr, если точка не найдена.
void JPSFindPoint (
    JPSNode ** result,
    JPSNode * point,
    signed char direction,
    bool * bool_bit_map,
    int bit_map_width,
    int bit_map_height,
    int end_x,
    int end_y
) {
    *result = nullptr;
    auto delta = JPSDirectionDelta[direction];
    auto is_diagonal = JPSDirectionIsDiagonal[direction];
    
    int current_x = point->x + delta.dx;
    int current_y = point->y + delta.dy;
    int steps_count = 1;
    
    while (
        /*is_in_bitmap(current_x, current_y, bit_map_width, bit_map_height) 
        &&*/ !bool_bit_map[current_y * bit_map_width + current_x]
    ) {
        if (JPSIsJumpPoint(current_x, current_y, direction, bool_bit_map, bit_map_width, bit_map_height, end_x, end_y))
        {
            int cost = point->from_begin_cost + JPSDirectionCost[direction] * steps_count;
            *result = new JPSNode {current_x, current_y, cost, true, direction, point->x, point->y};
            return;
        }
        
        if (is_diagonal)
        {
            auto check_direction = JPSDirectionDiagNeighbors[direction].first;
            auto check_direction_delta = JPSDirectionDelta[check_direction];
            int check_x = current_x + check_direction_delta.dx;
            int check_y = current_y + check_direction_delta.dy;
            while (
                /*is_in_bitmap(check_x, check_y, bit_map_width, bit_map_height) 
                &&*/ !bool_bit_map[check_y * bit_map_width + check_x]
            ) {
                if (JPSIsJumpPoint(check_x, check_y, check_direction, bool_bit_map, bit_map_width, bit_map_height, end_x, end_y))
                {
                    int cost = point->from_begin_cost + JPSDirectionCost[direction] * steps_count;
                    *result = new JPSNode {current_x, current_y, cost, true, direction, point->x, point->y};
                    return;
                }
                check_x += check_direction_delta.dx;
                check_y += check_direction_delta.dy;
            }
            
            check_direction = JPSDirectionDiagNeighbors[direction].second;
            check_direction_delta = JPSDirectionDelta[check_direction];
            check_x = current_x + check_direction_delta.dx;
            check_y = current_y + check_direction_delta.dy;
            while (
                /*is_in_bitmap(check_x, check_y, bit_map_width, bit_map_height) 
                &&*/ !bool_bit_map[check_y * bit_map_width + check_x]
            ) {
                if (JPSIsJumpPoint(check_x, check_y, check_direction, bool_bit_map, bit_map_width, bit_map_height, end_x, end_y))
                {
                    int cost = point->from_begin_cost + JPSDirectionCost[direction] * steps_count;
                    *result = new JPSNode {current_x, current_y, cost, true, direction, point->x, point->y};
                    return;
                }
                check_x += check_direction_delta.dx;
                check_y += check_direction_delta.dy;
            }
        }
        
        current_x += delta.dx;
        current_y += delta.dy;
        ++steps_count;
    }
    
    //return nullptr;
}

void JPSAddPointInOpenList (
    JPSNode * point,
    BinaryHeap <JPSNode> & open_heap,
    JPSNode ** p_nodes_array,
    int bit_map_width,
    int end_x,
    int end_y
) {
    int shift = point->y * bit_map_width + point->x;
    if (p_nodes_array[shift] == nullptr)
    {
        open_heap.push(point, point->from_begin_cost + JPSGetEvaluation(point->x, point->y, end_x, end_y));
        p_nodes_array[shift] = point;
    }
    else {
        if (p_nodes_array[shift]->is_opened && point->from_begin_cost < p_nodes_array[shift]->from_begin_cost)
        {
            p_nodes_array[shift]->from_begin_cost = point->from_begin_cost;
            p_nodes_array[shift]->direction_from_parent = point->direction_from_parent;
            p_nodes_array[shift]->parent_x = point->parent_x;
            p_nodes_array[shift]->parent_y = point->parent_y;
            open_heap.resort(p_nodes_array[shift], point->from_begin_cost - p_nodes_array[shift]->from_begin_cost);
        }
        delete point;
    }
}

// Открытый список не должен быть пустым. 
bool JPSCheckNeighbors (
    BinaryHeap <JPSNode> & open_heap,
    JPSNode ** p_nodes_array,
    bool * bool_bit_map,
    int bit_map_width,
    int bit_map_height,
    int end_x,
    int end_y
) {
    if (open_heap.front()->x == end_x && open_heap.front()->y == end_y) return true;
    
    auto node = open_heap.front();
    open_heap.pop();
    node->is_opened = false;
    
    std::thread * threads[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    JPSNode * points[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    
    threads[0] = new std::thread (JPSFindPoint, points, node, node->direction_from_parent, 
                                  bool_bit_map, bit_map_width, bit_map_height, end_x, end_y);
    
    auto direction_check = JPSDirectionChecks1[node->direction_from_parent].check;
    int check_x = node->x + JPSDirectionDelta[direction_check].dx;
    int check_y = node->y + JPSDirectionDelta[direction_check].dy;
    if (
        /*is_in_bitmap(check_x, check_y, bit_map_width, bit_map_height) 
        &&*/ bool_bit_map[check_y * bit_map_width + check_x]
    )
        threads[1] = new std::thread (JPSFindPoint, points + 1, node, JPSDirectionChecks1[node->direction_from_parent].neighbor, 
                                      bool_bit_map, bit_map_width, bit_map_height, end_x, end_y);
    
    direction_check = JPSDirectionChecks2[node->direction_from_parent].check;
    check_x = node->x + JPSDirectionDelta[direction_check].dx;
    check_y = node->y + JPSDirectionDelta[direction_check].dy;
    if (
        /*is_in_bitmap(check_x, check_y, bit_map_width, bit_map_height) 
        &&*/ bool_bit_map[check_y * bit_map_width + check_x]
    )
        threads[2] = new std::thread (JPSFindPoint, points + 2, node, JPSDirectionChecks2[node->direction_from_parent].neighbor, 
                                      bool_bit_map, bit_map_width, bit_map_height, end_x, end_y);
    
    if (JPSDirectionIsDiagonal[node->direction_from_parent])
    {
        auto add_directions = JPSDirectionDiagNeighbors[node->direction_from_parent];
        
        threads[3] = new std::thread (JPSFindPoint, points + 3, node, add_directions.first, 
                                      bool_bit_map, bit_map_width, bit_map_height, end_x, end_y);
        threads[4] = new std::thread (JPSFindPoint, points + 4, node, add_directions.second, 
                                      bool_bit_map, bit_map_width, bit_map_height, end_x, end_y);
    }
    
    for (int i = 0; i < 5; ++i)
        if (threads[i] != nullptr)
        {
            threads[i]->join();
            delete threads[i];
            if (points[i] != nullptr)
                JPSAddPointInOpenList(points[i], open_heap, p_nodes_array, bit_map_width, end_x, end_y);
        }
    
    return false;
}
/*
void add_pixels (JPSNode * current_node, std::forward_list <std::pair <int, int>> & patch)
{
    auto delta = JPSDirectionDelta[JPSOppositeDirection[current_node->direction_from_parent]];
    int add_x = current_node->x + delta.dx;
    int add_y = current_node->y + delta.dy;
    while (add_x != current_node->parent_x || add_y != current_node->parent_y)
    {
        patch.emplace_front <std::pair <int, int>> ({add_x, add_y});
        add_x += delta.dx;
        add_y += delta.dy;
    }
}*/

JPSFindResults JPSFindPatch (
    bool * bool_bit_map, 
    unsigned int width, unsigned int height,
    int start_x, int start_y,
    int end_x, int end_y
) {
    if (width == 0 || height == 0)
        return JPSFindResults {JPSFindStatus::BAD_MAP, {}};
    
    if (
        start_x < 0 || start_x >= static_cast <int> (width) || start_y < 0 || start_y >= static_cast <int> (height) 
        || end_x < 0 || end_x >= static_cast <int> (width) || end_y < 0 || end_y >= static_cast <int> (height) 
    )
        return JPSFindResults {JPSFindStatus::OUT_OF_MAP, {}};
    
    if (bool_bit_map[start_y * width + start_x] || bool_bit_map[end_y * width + end_x])
        return JPSFindResults {JPSFindStatus::NOT_ACCESSIBLE, {}};
    
    if (start_x == end_x && start_y == end_y)
        return JPSFindResults {JPSFindStatus::START_EQUAL_END, {}};
    
    const unsigned int pixels_count = width * height;
    JPSNode ** p_nodes_array = new JPSNode * [pixels_count]; // Точка x, y находится по смещению [y * width + x].
    for (unsigned int i = 0; i < pixels_count; ++i) p_nodes_array[i] = nullptr;
    BinaryHeap <JPSNode> open_heap(false);
    bool find_patch = false;
    JPSFindResults result {JPSFindStatus::NOT_FOUND, {}};
    
    JPSNode * start_node = new JPSNode {start_x, start_y, 0, false, -1, -1, -1};
    p_nodes_array[start_y * width + start_x] = start_node;
    
    std::thread * threads[8];
    JPSNode * thread_results[8];
    for (signed char i = 0; i < 8; ++i)
        threads[i] = new std::thread(JPSFindPoint, thread_results + i, start_node, i, bool_bit_map, width, height, end_x, end_y);
    for (signed char i = 0; i < 8; ++i)
    {
        threads[i]->join();
        delete threads[i];
        if (thread_results[i] != nullptr)
            JPSAddPointInOpenList(thread_results[i], open_heap, p_nodes_array, width, end_x, end_y);
    }
    
    while (!open_heap.empty() && !find_patch)
        find_patch = JPSCheckNeighbors(open_heap, p_nodes_array, bool_bit_map, width, height, end_x, end_y);
    
    if (find_patch)
    {
        result.status = JPSFindStatus::SUCCESS;
        
        auto current_node = open_heap.front();
        while (current_node->x != start_x || current_node->y != start_y)
        {
            result.patch.emplace_front <std::pair <int, int>> ({current_node->x, current_node->y});
            //add_pixels(current_node, result.patch);
            current_node = p_nodes_array[current_node->parent_y * width + current_node->parent_x];
        }
    }
    
    open_heap.clear();
    for (unsigned int i = 0; i < pixels_count; ++i)
        if (p_nodes_array[i] != nullptr) delete p_nodes_array[i];
    delete [] p_nodes_array;
    
    return result;
}
