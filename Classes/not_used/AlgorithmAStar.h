#ifndef ALGORITHMASTAR_H
#define ALGORITHMASTAR_H

#include <forward_list>
#include <vector>
#include <string>

enum class AStarFindStatus {SUCCESS, BAD_MAP, OUT_OF_MAP, NOT_ACCESSIBLE, START_EQUAL_END, NOT_FOUND};

std::string AStarStringFindStatus (AStarFindStatus status);

class AStarFindResults 
{
public:
    AStarFindStatus status;
    std::forward_list <std::pair <int, int>> patch;
};

// Сама стартовая точка не входит в результат. 
AStarFindResults AStarFindPatch (
    const std::vector <std::vector <bool>> & bit_map, 
    int start_x, int start_y,
    int end_x, int end_y
);

#endif // ALGORITHMASTAR_H
