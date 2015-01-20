#ifndef ATTACKDIRECTION_H
#define ATTACKDIRECTION_H

#include "AttackState.h"

// Это состояние только для игрового персонажа.

class AttackDirection : public AttackState
{
public:
    AttackDirection (Vec2 direction);
    virtual ~AttackDirection () = default;
    
    virtual void check_input (MapActor *) override;
    virtual void actions (float dt, MapActor *) override;
    
protected:
    Vec2 _direction;
};

#endif // ATTACKDIRECTION_H
