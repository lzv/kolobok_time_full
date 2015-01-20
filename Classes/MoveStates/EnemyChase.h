#ifndef ENEMYCHASE_H
#define ENEMYCHASE_H

#include "MoveState.h"
#include <forward_list>

/*
Враг преследует цель, не переключаясь на другие состояния движения.
Угол поворота постоянно меняется, что бы враг смотрел прямо на персонажа.
*/

class EnemyChase : public MoveState
{
public:
    virtual void check_input (MapActor * en) override;
    virtual void update_character (float dt, MapActor * en) override;
    
    EnemyChase (MapActor * target);
    virtual ~EnemyChase () = default;
    
protected:
    static const float FindPatchInterval;
    
    MapActor * _target;
    bool _initialized;
    float _time_counter;
    std::forward_list <std::pair <int, int>> _patch;
};

#endif // ENEMYCHASE_H
