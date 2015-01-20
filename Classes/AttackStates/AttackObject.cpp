#include "AttackObject.h"
#include "AttackDirectionWhileChase.h"
#include "NoAttack.h"
#include "../InputManager.h"
#include "../MapActors/Enemy.h"
#include "../Scenes/PlayScreen.h"
#include "../CharWeapon.h"

AttackObject::AttackObject (MapActor * target) : _target(target) {}

void AttackObject::check_input (MapActor * ch)
{
    if (InputManager::I().is_mouse_right_key())
    {
        auto obj = new AttackDirectionWhileChase(
            Vec2 {ch->getPosition(), InputManager::I().get_mouse_pos() - ch->_main_screen->getLayersPos()}, 
            _target
        );
        ch->replace_attack_state(obj);
    }
    else if (InputManager::I().is_ctrl_play_rules() || InputManager::I().check_NAPR_play_rules().is_true())
    {
        auto obj = new NoAttack();
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
            auto obj = new NoAttack();
            ch->replace_attack_state(obj);
        }
    }
}

void AttackObject::actions (float, MapActor * ch)
{
    if (ch->getPosition().distance(_target->getPosition()) < ch->_current_weapon->_radius + 50.0f)
    {
        ch->set_rotate_angle(get_angle_of_vector(Vec2 {ch->getPosition(), _target->getPosition()}));
        ch->_current_weapon->start_attack(ch);
    }
    else {
        ch->_current_weapon->stop_attack();
    }
}

void AttackObject::on_target_dead (MapActor * ch)
{
    auto obj = new NoAttack();
    ch->replace_attack_state(obj);
}
