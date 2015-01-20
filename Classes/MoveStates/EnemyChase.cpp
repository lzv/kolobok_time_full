#include "EnemyChase.h"
#include "../MapActors/MapActor.h"
#include "../Scenes/PlayScreen.h"
#include "../Tools.h"

const float EnemyChase::FindPatchInterval = 0.4f;

EnemyChase::EnemyChase (MapActor * target)
    : _target(target), _initialized(false), _time_counter(-1.0f)
{}

void EnemyChase::check_input (MapActor * en)
{
    if (!_initialized && _time_counter <= 0.0f)
    {
        auto result = en->_main_screen->find_patch_from_MapActor(en, _target->getPosition());
        _time_counter = FindPatchInterval;
        
        if (result.status == JPSFindStatus::SUCCESS)
        {
            _patch.swap(result.patch);
            _initialized = true;
        }
    }
}

void EnemyChase::update_character (float dt, MapActor * en)
{
    if (_time_counter > 0.0f) _time_counter -= dt;
    
    if (_initialized)
    {
        if (_time_counter <= 0.0f)
        {
            _patch.clear();
            _initialized = false;
            check_input(en);
        }
        
        if (!_patch.empty())
        {
            if (step_along_patch(en, _patch, dt))
            {
                en->set_position(en->getPosition()); // Установка позиции с доп. действиями.
                en->set_rotate_angle(get_angle_of_vector(Vec2 {en->getPosition(), _target->getPosition()}));
            }
            else {
                _patch.clear();
                _initialized = false;
            }
        }
    }
}
