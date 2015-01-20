#ifndef ENEMYWAIT_H
#define ENEMYWAIT_H

#include "MoveState.h"

/*
В режиме ожидания враг выбирает случайное направление и идет по нему 200 пикселей, или пока не натолкнется
на препятствие. Если он отдалится от своей базовой позиции более, чем на 300, он переключается в состояние 
EnemyReturnToBase. Если персонаж подходит ближе 500 пикселов, состояние преключается на его преследование.
*/

class EnemyWait : public MoveState
{
public:
    virtual void check_input (MapActor * en) override;
    virtual void update_character (float dt, MapActor * en) override;
    
    EnemyWait (float base_x, float base_y);
    virtual ~EnemyWait () = default;
    
protected:
    static const float DistanceInLine;
    static const float DistanceFromBase;
    static const float DistanceToCharacter;
    
    std::function <float ()> _get_rand_angle;
    float _base_x, _base_y;
    bool _need_new_direction;
    Point _direction;
    float _distance;
};

#endif // ENEMYWAIT_H
