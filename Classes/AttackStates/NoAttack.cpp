#include "NoAttack.h"
#include "AttackObject.h"
#include "AttackDirection.h"
#include "../InputManager.h"
#include "../MapActors/Enemy.h"
#include "../Scenes/PlayScreen.h"
#include "../CharWeapon.h"

void NoAttack::check_input (MapActor * ch)
{
    // ПКМ имеет больший приоритет.
    if (InputManager::I().is_mouse_right_key())
    {
        auto obj = new AttackDirection(
            Vec2{ch->getPosition(), 
            InputManager::I().get_mouse_pos() - ch->_main_screen->getLayersPos()}
        );
        ch->replace_attack_state(obj);
    }
    else if (InputManager::I().is_mouse_left_key_play_rules())
    {
        auto point = InputManager::I().get_mouse_pos();
        auto point_type = ch->_main_screen->getPointType(point);
        
        if (point_type == PointType::ENEMY)
        {
            auto obj = new AttackObject(ch->_main_screen->getLastFoundEnemy());
            ch->replace_attack_state(obj);
        }
    }
}

void NoAttack::actions (float, MapActor * ch)
{
    ch->_current_weapon->stop_attack();
}
