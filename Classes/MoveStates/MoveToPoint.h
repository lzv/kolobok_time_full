#ifndef MOVETOPOINT_H
#define MOVETOPOINT_H

#include "MoveState.h"
#include "../AlgorithmJPS_v2.h"

class MoveToPoint : public MoveState
{
public:
    virtual void check_input (MapActor * ch) override;
    virtual void update_character (float dt, MapActor * ch) override;
    
    MoveToPoint (float x, float y);
    virtual ~MoveToPoint () = default;
    
protected:
    static const float FindPatchInterval;
    
    float _x, _y;
    bool _initialized;
    std::forward_list <std::pair <int, int>> _patch;
    float _time_counter;
};

#endif // MOVETOPOINT_H
