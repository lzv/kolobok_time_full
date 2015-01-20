#ifndef MOVECHASE_H
#define MOVECHASE_H

#include "MoveState.h"
#include "../AlgorithmJPS_v2.h"

class MoveChase : public MoveState
{
public:
    virtual void check_input (MapActor * ch) override;
    virtual void update_character (float dt, MapActor * ch) override;
    virtual void on_target_dead (MapActor * ch) override;
    
    MoveChase (MapActor * target);
    virtual ~MoveChase () = default;
    
protected:
    static const float FindPatchInterval;
    
    MapActor * _target;
    bool _initialized;
    std::forward_list <std::pair <int, int>> _patch;
    float _time_counter;
};

#endif // MOVECHASE_H
