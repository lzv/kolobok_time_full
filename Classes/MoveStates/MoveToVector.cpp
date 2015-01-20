#include "MoveToVector.h"
#include "StandStill.h"
#include "../InputManager.h"
#include "../MapActors/MapActor.h"
#include "../Scenes/PlayScreen.h"
#include "../Tools.h"

MoveToVector::MoveToVector (const Vec2 & direction)
    : MoveState(), _direction(direction)
{
    _direction.normalize();
}

void MoveToVector::check_input (MapActor * ch)
{
    NAPR napr = InputManager::I().check_NAPR_play_rules();
    if (napr.is_true())
    {
        _direction = napr.get_direction();
        ch->set_rotate_angle(get_angle_of_vector(_direction));
    }
    else {
        auto obj = new StandStill();
        ch->replace_move_state(obj);
    }
}

void MoveToVector::update_character (float dt, MapActor * ch)
{
    auto prev_position = ch->getPosition();
    auto new_position = prev_position + _direction * (dt * ch->_base_speed);
    ch->setPosition(new_position); // Изменение только позиции, без дополнительных действий.
    if (ch->_main_screen->isIntersectMapActorWithMoveObstructions(ch))
    {
        ch->setPosition(prev_position); // Возврат предыдущей позиции 
    }
    else {
        ch->set_position(new_position); // Установка позиции с доп. действиями, разными для разных наследников MapActor. 
    }
}
