#ifndef ENEMYATTACK_H
#define ENEMYATTACK_H

#include "AttackState.h"

// Это состояние только для врага.

class EnemyAttack : public AttackState
{
public:
    EnemyAttack (MapActor * target);
    virtual ~EnemyAttack () = default;
    
    virtual void check_input (MapActor *) override;
    virtual void actions (float dt, MapActor *) override;
    
protected:
    MapActor * _target;
};

#endif // ENEMYATTACK_H
