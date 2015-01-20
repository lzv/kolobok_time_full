#include "AttackDirectionWhileChase.h"
#include "AttackObject.h"
#include "AttackDirection.h"
#include "../InputManager.h"
#include "../MapActors/Enemy.h"
#include "../Scenes/PlayScreen.h"
#include "../CharWeapon.h"
#include "../Tools.h"

AttackDirectionWhileChase::AttackDirectionWhileChase (Vec2 direction, MapActor * target)
    : _direction(direction), _target(target)
{
    _direction.normalize();
}

void AttackDirectionWhileChase::check_input (MapActor * ch)
{
    if (InputManager::I().is_mouse_right_key())
    {
        _direction = Vec2(ch->getPosition(), InputManager::I().get_mouse_pos() - ch->_main_screen->getLayersPos());
        _direction.normalize();
        
        if (InputManager::I().is_ctrl_play_rules() || InputManager::I().check_NAPR_play_rules().is_true())
        {
            auto obj = new AttackDirection(_direction);
            ch->replace_attack_state(obj);
        }
        else if (InputManager::I().is_mouse_left_key_play_rules())
        {
            auto point = InputManager::I().get_mouse_pos();
            auto point_type = ch->_main_screen->getPointType(point);
            
            if (point_type == PointType::ENEMY)
            {
                _target = ch->_main_screen->getLastFoundEnemy();
            }
            else if (
                point_type == PointType::NOT_ACCESSIBLE 
                || point_type == PointType::ACCESSIBLE 
                || point_type == PointType::OBJECT
            ) {
                auto obj = new AttackDirection(_direction);
                ch->replace_attack_state(obj);
            }
        }
    }
    else {
        auto obj = new AttackObject(_target);
        ch->replace_attack_state(obj);
    }
}

void AttackDirectionWhileChase::actions (float dt, MapActor * ch)
{
    ch->set_rotate_angle(get_angle_of_vector(_direction));
    ch->_current_weapon->start_attack(ch);
}

void AttackDirectionWhileChase::on_target_dead (MapActor * ch)
{
    auto obj = new AttackDirection(_direction);
    ch->replace_attack_state(obj);
}
