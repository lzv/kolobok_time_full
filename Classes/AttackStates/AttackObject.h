#ifndef ATTACKOBJECT_H
#define ATTACKOBJECT_H

#include "AttackState.h"

// Это состояние только для игрового персонажа.

class AttackObject : public AttackState
{
public:
    AttackObject (MapActor * target);
    virtual ~AttackObject () = default;
    
    virtual void check_input (MapActor *) override;
    virtual void actions (float dt, MapActor *) override;
    virtual void on_target_dead (MapActor * ch) override;
    
protected:
    MapActor * _target;
};

#endif // ATTACKOBJECT_H
