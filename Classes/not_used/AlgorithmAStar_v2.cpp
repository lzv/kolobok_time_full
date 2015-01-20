#include "AlgorithmAStar.h"
#include "BinaryHeap.h"
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

// Реализация A* с вручную написанной min-кучей. 

// По сравнению с реализацией в файле AlgorithmAStar_v1.cpp, реализация с кучей работает более 
// чем в 10 раз быстрее. Но при поиске через всю карту все равно затрачивается пара секунд.
// Это недопустимо медленно. Решено реализовывать алгоритм Jump Point Search, конечно же, 
// с использованием быстрой кучи :) 

struct AStarNode {int x, y;};

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
    BinaryHeap <AStarNode> & open_heap, 
    const std::vector <std::vector <bool>> & bit_map,
    int bit_map_width,
    int bit_map_height,
    int end_x,
    int end_y,
    AStarNodeInfo * nodes_info
) {
    int current_x = open_heap.front()->x;
    int current_y = open_heap.front()->y;
    int current_shift = current_y * bit_map_width + current_x;
    open_heap.pop();
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
                    open_heap.resort(nodes_info[n_shift].pointer, delta_cost);
                }
            }
            else {
                nodes_info[n_shift].status = AStarPointStatus::OPENED;
                nodes_info[n_shift].direction_to_parent = AStarOppositeDirection[i];
                nodes_info[n_shift].from_begin_cost = n_from_begin_cost;
                AStarNode * point = new AStarNode {nx, ny};
                open_heap.push(point, n_from_begin_cost + AStarGetEvaluation(nx, ny, end_x, end_y));
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
    
    BinaryHeap <AStarNode> open_heap(true);
    bool patch_found = false;
    AStarFindResults result {AStarFindStatus::NOT_FOUND, {}};
    
    AStarNode * start_point = new AStarNode {start_x, start_y};
    open_heap.push(start_point, AStarGetEvaluation(start_x, start_y, end_x, end_y));
    int pointer_shift = start_y * width + start_x;
    nodes_info[pointer_shift].pointer = start_point;
    nodes_info[pointer_shift].status = AStarPointStatus::OPENED;
    nodes_info[pointer_shift].from_begin_cost = 0;
    
    while (!open_heap.empty() && !patch_found)
        patch_found = AStarStep(open_heap, bit_map, width, height, end_x, end_y, nodes_info);
    
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
    
    open_heap.clear();
    delete [] nodes_info;
    
    return result;
}
