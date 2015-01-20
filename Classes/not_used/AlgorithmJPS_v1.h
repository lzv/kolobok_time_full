#ifndef ALGORITHMJPS_H
#define ALGORITHMJPS_H

#include <forward_list>
#include <vector>
#include <string>

enum class JPSFindStatus {SUCCESS, BAD_MAP, OUT_OF_MAP, NOT_ACCESSIBLE, START_EQUAL_END, NOT_FOUND};

std::string JPSStringFindStatus (JPSFindStatus status);

class JPSFindResults 
{
public:
    JPSFindStatus status;
    std::forward_list <std::pair <int, int>> patch;
};

// Сама стартовая точка не входит в результат. 
JPSFindResults JPSFindPatch (
    const std::vector <std::vector <bool>> & bit_map, 
    int start_x, int start_y,
    int end_x, int end_y
);

#endif // ALGORITHMJPS_H
