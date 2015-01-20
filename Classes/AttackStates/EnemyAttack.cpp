#include "EnemyAttack.h"
#include "../MapActors/Enemy.h"
#include "../Scenes/PlayScreen.h"
#include "../CharWeapon.h"
#include "../Tools.h"

EnemyAttack::EnemyAttack (MapActor * target) : _target(target) {}

void EnemyAttack::check_input (MapActor *) {}

void EnemyAttack::actions (float dt, MapActor * en)
{
    if (en->_current_weapon->_type == WeaponType::FIRE && en->_current_weapon->bullets_count_at(en) == 0)
    {
        Enemy * obj = dynamic_cast <Enemy *> (en);
        if (obj) obj->change_weapon();
    }
    
    if (en->getPosition().distance(_target->getPosition()) < en->_current_weapon->_radius + 50.0f)
    {
        en->set_rotate_angle(get_angle_of_vector(Vec2 {en->getPosition(), _target->getPosition()}));
        en->_current_weapon->start_attack(en);
    }
    else {
        en->_current_weapon->stop_attack();
    }
}
