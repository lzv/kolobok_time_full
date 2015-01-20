#include "AttackDirection.h"
#include "NoAttack.h"
#include "AttackDirectionWhileChase.h"
#include "../InputManager.h"
#include "../MapActors/Enemy.h"
#include "../CharWeapon.h"
#include "../Scenes/PlayScreen.h"
#include "../Tools.h"

AttackDirection::AttackDirection (Vec2 direction) : _direction(direction)
{
    _direction.normalize();
}

void AttackDirection::check_input (MapActor * ch)
{
    if (InputManager::I().is_mouse_right_key())
    {
        _direction = Vec2(ch->getPosition(), InputManager::I().get_mouse_pos() - ch->_main_screen->getLayersPos());
        _direction.normalize();
        
        if (InputManager::I().is_mouse_left_key_play_rules())
        {
            auto point = InputManager::I().get_mouse_pos();
            auto point_type = ch->_main_screen->getPointType(point);
            
            if (point_type == PointType::ENEMY)
            {
                auto obj = new AttackDirectionWhileChase(_direction, ch->_main_screen->getLastFoundEnemy());
                ch->replace_attack_state(obj);
            }
        }
    }
    else {
        auto obj = new NoAttack();
        ch->replace_attack_state(obj);
    }
}

void AttackDirection::actions (float dt, MapActor * ch)
{
    ch->set_rotate_angle(get_angle_of_vector(_direction));
    ch->_current_weapon->start_attack(ch);
}
