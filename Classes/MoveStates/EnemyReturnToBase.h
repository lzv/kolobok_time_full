#ifndef ENEMYRETURNTOBASE_H
#define ENEMYRETURNTOBASE_H

#include "MoveState.h"
#include <forward_list>

/*
Возврат на базовую точку с использованием поиска пути. После возвращения переключение в состояние EnemyWait.
Если персонаж подходит ближе 500 пикселов, состояние преключается на его преследование.
*/

class EnemyReturnToBase : public MoveState
{
public:
    virtual void check_input (MapActor * en) override;
    virtual void update_character (float dt, MapActor * en) override;
    
    EnemyReturnToBase (float base_x, float base_y);
    virtual ~EnemyReturnToBase () = default;
    
protected:
    static const float DistanceToCharacter;
    
    float _base_x, _base_y;
    bool _initialized;
    std::forward_list <std::pair <int, int>> _patch;
};

#endif // ENEMYRETURNTOBASE_H
