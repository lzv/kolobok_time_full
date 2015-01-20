#ifndef ATTACKDIRECTIONWHILECHASE_H
#define ATTACKDIRECTIONWHILECHASE_H

#include "AttackState.h"

// Это состояние только для игрового персонажа.

class AttackDirectionWhileChase : public AttackState
{
public:
    AttackDirectionWhileChase (Vec2 direction, MapActor * target);
    virtual ~AttackDirectionWhileChase () = default;
    
    virtual void check_input (MapActor *) override;
    virtual void actions (float dt, MapActor *) override;
    virtual void on_target_dead (MapActor * ch) override;
    
protected:
    Vec2 _direction;
    MapActor * _target;
};

#endif // ATTACKDIRECTIONWHILECHASE_H
