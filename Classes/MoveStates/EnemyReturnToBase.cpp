#include "EnemyReturnToBase.h"
#include "EnemyWait.h"
#include "EnemyChase.h"
#include "../MapActors/Character.h"
#include "../Scenes/PlayScreen.h"

const float EnemyReturnToBase::DistanceToCharacter = 400.0f;

EnemyReturnToBase::EnemyReturnToBase (float base_x, float base_y)
    : _base_x(base_x), _base_y(base_y), _initialized(false)
{}

void EnemyReturnToBase::check_input (MapActor * en)
{
    if (!_initialized)
    {
        auto result = en->_main_screen->find_patch_from_MapActor(en, Point(_base_x, _base_y));
        if (result.status == JPSFindStatus::SUCCESS)
        {
            _initialized = true;
            _patch.swap(result.patch);
        }
        else {
            auto obj = new EnemyWait(_base_x, _base_y);
            en->replace_move_state(obj);
            return;
        }
    }
    
    if (en->getPosition().distance(en->_main_screen->_character->getPosition()) < DistanceToCharacter)
    {
        auto obj = new EnemyChase(en->_main_screen->_character);
        en->replace_move_state(obj);
    }
}

void EnemyReturnToBase::update_character (float dt, MapActor * en)
{
    if (_initialized)
    {
        if (_patch.empty())
        {
            auto obj = new EnemyWait(_base_x, _base_y);
            en->replace_move_state(obj);
            return;
        }
        
        if (step_along_patch(en, _patch, dt, true))
        {
            en->set_position(en->getPosition()); // Установка позиции с доп. действиями, разными для разных наследников MapActor.
            if (_patch.empty())
            {
                auto obj = new EnemyWait(_base_x, _base_y);
                en->replace_move_state(obj);
            }
        }
        else {
            auto obj = new EnemyWait(_base_x, _base_y);
            en->replace_move_state(obj);
        }
    }
}
