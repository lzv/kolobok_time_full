#include "MoveToPoint.h"
#include "StandStill.h"
#include "MoveChase.h"
#include "MoveToVector.h"
#include "../InputManager.h"
#include "../Scenes/PlayScreen.h"
#include "../MapActors/MapActor.h"
#include "../MapActors/Enemy.h"
#include "../MapObject.h"

const float MoveToPoint::FindPatchInterval = 0.1f;

MoveToPoint::MoveToPoint (float x, float y)
    : MoveState(), _x(x), _y(y), _initialized(false), _time_counter(FindPatchInterval)
{}

void MoveToPoint::check_input (MapActor * ch)
{
    if (!_initialized)
    {
        auto result = ch->_main_screen->find_patch_from_MapActor(ch, Point(_x, _y));
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
            
            if ((fabs(_x - point.x) > 0.5f || fabs(_y - point.y) > 0.5f) && _time_counter <= 0.0f)
            {
                auto result = ch->_main_screen->find_patch_from_MapActor(ch, point);
                _time_counter = FindPatchInterval;
                
                if (result.status == JPSFindStatus::SUCCESS)
                {
                    _x = point.x;
                    _y = point.y;
                    _patch.swap(result.patch);
                }
                else {
                    auto obj = new StandStill();
                    ch->replace_move_state(obj);
                }
            }
        }
        else if (point_type == PointType::ENEMY)
        {
            auto target = ch->_main_screen->getLastFoundEnemy();
            auto obj = new MoveChase(target);
            ch->replace_move_state(obj);
        }
        else if (point_type == PointType::NOT_ACCESSIBLE)
        {
            auto obj = new StandStill();
            ch->replace_move_state(obj);
        }
    }
    else if (InputManager::I().is_ctrl_play_rules())
    {
        auto obj = new StandStill();
        ch->replace_move_state(obj);
    }
    else {
        NAPR napr = InputManager::I().check_NAPR_play_rules();
        if (napr.is_true())
        {
            auto obj = new MoveToVector(napr.get_direction());
            ch->replace_move_state(obj);
        }
    }
}

void MoveToPoint::update_character (float dt, MapActor * ch)
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
        
        if (step_along_patch(ch, _patch, dt, true))
        {
            ch->set_position(ch->getPosition()); // Установка позиции с доп. действиями, разными для разных наследников MapActor.
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
