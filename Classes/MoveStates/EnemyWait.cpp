#include "EnemyWait.h"
#include "EnemyChase.h"
#include "EnemyReturnToBase.h"
#include "../Scenes/PlayScreen.h"
#include "../MapActors/Character.h"
#include "../Tools.h"

const float EnemyWait::DistanceInLine = 200.0f;
const float EnemyWait::DistanceFromBase = 300.0f;
const float EnemyWait::DistanceToCharacter = 400.0f;

EnemyWait::EnemyWait (float base_x, float base_y)
    : _get_rand_angle([]() -> float {return (static_cast <float> (rand()) / RAND_MAX) * degree_to_rad(359.99999f);}),
    _base_x(base_x), _base_y(base_y), _need_new_direction(true)
{}

void EnemyWait::check_input (MapActor * en)
{
    if (en->getPosition().distance(en->_main_screen->_character->getPosition()) < DistanceToCharacter)
    {
        auto obj = new EnemyChase(en->_main_screen->_character);
        en->replace_move_state(obj);
    }
    else if (en->getPosition().distance(Point(_base_x, _base_y)) > DistanceFromBase)
    {
        auto obj = new EnemyReturnToBase(_base_x, _base_y);
        en->replace_move_state(obj);
    }
}

void EnemyWait::update_character (float dt, MapActor * en)
{
    if (_need_new_direction)
    {
        float angle = _get_rand_angle();
        _direction.set(cos(angle), -sin(angle));
        _need_new_direction = false;
        _distance = DistanceInLine;
        en->set_rotate_angle(rad_to_degree(angle));
    }
    
    if (_distance > 0)
    {
        float step_distance = en->_base_speed * dt;
        auto old_pos = en->getPosition();
        auto new_pos = old_pos + _direction * step_distance;
        en->setPosition(new_pos);       // Изменение только позиции, без дополнительных действий.
        if (en->_main_screen->isIntersectMapActorWithMoveObstructions(en))
        {
            en->setPosition(old_pos);   // Возврат предыдущей позиции
            _need_new_direction = true;
        }
        else {
            en->set_position(new_pos);  // Установка позиции с доп. действиями, разными для разных наследников MapActor. 
            _distance -= step_distance;
        }
    }
    else {
        _need_new_direction = true;
    }
}
