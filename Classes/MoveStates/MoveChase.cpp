#include "MoveChase.h"
#include "StandStill.h"
#include "MoveToPoint.h"
#include "MoveToVector.h"
#include "../InputManager.h"
#include "../MapActors/MapActor.h"
#include "../MapActors/Enemy.h"
#include "../Scenes/PlayScreen.h"
#include "../MapObject.h"
#include "../Tools.h"

const float MoveChase::FindPatchInterval = 0.5f;

MoveChase::MoveChase (MapActor * target)
    : MoveState(), _target(target), _initialized(false), _time_counter(FindPatchInterval)
{}

void MoveChase::check_input (MapActor * ch)
{
    if (_target == nullptr) 
    {
        auto obj = new StandStill();
        ch->replace_move_state(obj);
        return;
    }
    
    if (!_initialized)
    {
        auto result = ch->_main_screen->find_patch_from_MapActor(ch, _target->getPosition());
        if (result.status == JPSFindStatus::SUCCESS)
        {
            _initialized = true;
            _patch.swap(result.patch);
        }
        else {
            auto obj = new StandStill();
            ch->replace_move_state(obj);
            return;
        }
    }
    
    if (InputManager::I().is_mouse_left_key_play_rules())
    {
        auto point = InputManager::I().get_mouse_pos();
        auto point_type = ch->_main_screen->getPointType(point);
        
        if (point_type == PointType::ACCESSIBLE || point_type == PointType::OBJECT)
        {
            point = point - ch->_main_screen->getLayersPos();
            if (point_type == PointType::OBJECT)
            {
                auto map_obj = ch->_main_screen->getLastFoundMapObject();
                if (map_obj) point = map_obj->getBoundingBox().origin + map_obj->getAnchorPointInPoints();
            }
            
            auto obj = new MoveToPoint(point.x, point.y);
            ch->replace_move_state(obj);
            return;
        }
        else if (point_type == PointType::ENEMY)
        {
            auto en = ch->_main_screen->getLastFoundEnemy();
            if (en != _target)
            {
                auto result = ch->_main_screen->find_patch_from_MapActor(ch, en->getPosition());
                _time_counter = FindPatchInterval;
                
                if (result.status == JPSFindStatus::SUCCESS)
                {
                    _target = en;
                    _patch.swap(result.patch);
                }
                else {
                    auto obj = new StandStill();
                    ch->replace_move_state(obj);
                    return;
                }
            }
        }
        else if (point_type == PointType::NOT_ACCESSIBLE)
        {
            auto obj = new StandStill();
            ch->replace_move_state(obj);
            return;
        }
    }
    else if (InputManager::I().is_ctrl_play_rules())
    {
        auto obj = new StandStill();
        ch->replace_move_state(obj);
        return;
    }
    else {
        NAPR napr = InputManager::I().check_NAPR_play_rules();
        if (napr.is_true())
        {
            auto obj = new MoveToVector(napr.get_direction());
            ch->replace_move_state(obj);
            return;
        }
    }
    
    if (_time_counter <= 0.0f && ch->getPosition().distance(_target->getPosition()) < 500.0f)
    {
        auto result = ch->_main_screen->find_patch_from_MapActor(ch, _target->getPosition());
        _time_counter = FindPatchInterval;
        
        if (result.status == JPSFindStatus::SUCCESS)
        {
            _patch.swap(result.patch);
        }
        else {
            auto obj = new StandStill();
            ch->replace_move_state(obj);
        }
    }
}

void MoveChase::update_character (float dt, MapActor * ch)
{
    if (_initialized)
    {
        if (_time_counter > 0.0f) _time_counter -= dt;
        
        if (_patch.empty())
        {
            auto obj = new StandStill();
            ch->replace_move_state(obj);
            return;
        }
        
        if (step_along_patch(ch, _patch, dt))
        {
            ch->set_position(ch->getPosition()); // Установка позиции с доп. действиями, разными для разных наследников MapActor.
            ch->set_rotate_angle(get_angle_of_vector(Vec2{ch->getPosition(), _target->getPosition()}));
            
            if (_patch.empty())
            {
                auto obj = new StandStill();
                ch->replace_move_state(obj);
            }
        }
        else {
            auto obj = new StandStill();
            ch->replace_move_state(obj);
        }
    }
}

void MoveChase::on_target_dead (MapActor * ch)
{
    auto obj = new StandStill();
    ch->replace_move_state(obj);
}
