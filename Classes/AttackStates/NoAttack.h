#ifndef NOATTACK_H
#define NOATTACK_H

#include "AttackState.h"

// Это состояние только для игрового персонажа.

class NoAttack : public AttackState
{
public:
    NoAttack () = default;
    virtual ~NoAttack () = default;
    
    virtual void check_input (MapActor *) override;
    virtual void actions (float dt, MapActor *) override;
};

#endif // NOATTACK_H
