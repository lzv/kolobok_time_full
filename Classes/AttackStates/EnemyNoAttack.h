#ifndef ENEMYNOATTACK_H
#define ENEMYNOATTACK_H

#include "AttackState.h"

// Это состояние только для врага.

class EnemyNoAttack : public AttackState
{
public:
    EnemyNoAttack () = default;
    virtual ~EnemyNoAttack () = default;
    
    virtual void check_input (MapActor *) override;
    virtual void actions (float dt, MapActor *) override;
    
protected:
    static const float DistanceToCharacter;
};

#endif // ENEMYNOATTACK_H
