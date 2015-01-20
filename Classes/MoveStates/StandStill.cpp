#include "StandStill.h"
#include "MoveToVector.h"
#include "MoveToPoint.h"
#include "MoveChase.h"
#include "../MapActors/Enemy.h"
#include "../DBManager.h"
#include "../InputManager.h"
#include "../Scenes/PlayScreen.h"
#include "../MapObject.h"

void StandStill::check_input (MapActor * ch)
{
    if (InputManager::I().is_mouse_left_key_play_rules())
    {
        auto point = InputManager::I().get_mouse_pos();
        auto point_type = ch->_main_screen->getPointType(point);
        if (point_type == PointType::ACCESSIBLE)
        {
            point = point - ch->_main_screen->getLayersPos();
            auto obj = new MoveToPoint(point.x, point.y);
            ch->replace_move_state(obj);
        }
        else if (point_type == PointType::OBJECT)
        {
            auto map_obj = ch->_main_screen->getLastFoundMapObject();
            if (map_obj) /* по идее всегда не nullptr */
            {
                // Объекты MapObject не вращаются.
                point = map_obj->getBoundingBox().origin + map_obj->getAnchorPointInPoints();
                auto obj = new MoveToPoint(point.x, point.y);
                ch->replace_move_state(obj);
            }
        }
        else if (point_type == PointType::ENEMY)
        {
            auto target = ch->_main_screen->getLastFoundEnemy();
            auto obj = new MoveChase(target);
            ch->replace_move_state(obj);
        }
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

void StandStill::update_character (float, MapActor *) {}
