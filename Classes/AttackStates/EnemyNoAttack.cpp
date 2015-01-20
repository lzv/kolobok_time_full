#include "EnemyNoAttack.h"
#include "EnemyAttack.h"
#include "../MapActors/Character.h"
#include "../Scenes/PlayScreen.h"
#include "../CharWeapon.h"

const float EnemyNoAttack::DistanceToCharacter = 400.0f;

void EnemyNoAttack::check_input (MapActor * en)
{
    if (en->getPosition().distance(en->_main_screen->_character->getPosition()) < DistanceToCharacter)
    {
        auto obj = new EnemyAttack(en->_main_screen->_character);
        en->replace_attack_state(obj);
    }
}

void EnemyNoAttack::actions (float dt, MapActor * en)
{
    en->_current_weapon->stop_attack();
}
