#include "Enemy.h"
#include "../ObjectsParams.h"
#include "../MoveStates/EnemyWait.h"
#include "../AttackStates/EnemyNoAttack.h"
#include "../Scenes/PlayScreen.h"
#include "../Tools.h"

Enemy * Enemy::create (const EnemyDBParams & params, PlayScreen * screen)
{
    Enemy * p = new Enemy(screen);
    if (p && p->init(params))
    {
        p->autorelease();
        return p;
    }
    else {
        delete p;
        return nullptr;
    }
}

Enemy::Enemy (PlayScreen * screen) : MapActor(120.0f, screen) {}

Enemy::~Enemy () {}

bool Enemy::init (const EnemyDBParams & params)
{
    auto type_params = get_enemy_map_params (params.type_code);
    if (!MapActor::init(type_params.img_file_name, params.weapon1, true)) return false;
    
    _move_state = new EnemyWait(params.base_x, params.base_y);
    _attack_state = new EnemyNoAttack();
    
    _db_id = params.id;
    _second_weapon_code = params.weapon2;
    _name_for_user = type_params.name_for_user;
    _strength = params.strength;
    _accuracy = params.accuracy;
    
    set_rotate_angle(params.angle);
    
    return true;
}

void Enemy::set_position (float x, float y)
{
    setPosition(x, y);
    DBManager::I().update_enemy_field(_db_id, "pos_x", NumberToString(static_cast <int> (x)));
    DBManager::I().update_enemy_field(_db_id, "pos_y", NumberToString(static_cast <int> (y)));
}

void Enemy::set_position (const Vec2 & position)
{
    set_position(position.x, position.y);
}

unsigned int Enemy::get_current_hp ()
{
    return get_max_hp() - DBManager::I().get_enemy_field <unsigned int> (_db_id, "damage");
}

unsigned int Enemy::get_max_hp ()
{
    return 10 + 5 * get_strength();
}

void Enemy::add_hp (unsigned int count)
{
    int hits_damage = DBManager::I().get_enemy_field <int> (_db_id, "damage");
    int real_add_hp = std::min(hits_damage, static_cast <int> (count));
    hits_damage -= static_cast <int> (count);
    if (hits_damage < 0) hits_damage = 0;
    DBManager::I().update_enemy_field(_db_id, "damage", NumberToString(hits_damage));
    update_hits_line();
    set_popup_string(std::string("+") + NumberToString(real_add_hp), Color3B{0x00, 0xff, 0x00});
}

void Enemy::add_damage (unsigned int damage)
{
    unsigned int hits_damage = DBManager::I().get_enemy_field <unsigned int> (_db_id, "damage") + damage;
    DBManager::I().update_enemy_field(_db_id, "damage", NumberToString(hits_damage));
    update_hits_line();
    set_popup_string(std::string("-") + NumberToString(damage), Color3B{0xff, 0x00, 0x00});
}

unsigned int Enemy::get_strength ()
{
    return _strength;
}

unsigned int Enemy::get_accuracy ()
{
    return _accuracy;
}

void Enemy::change_weapon ()
{
    if (!_second_weapon_code.empty())
    {
        MapActor::set_new_weapon(_second_weapon_code);
        DBManager::I().update_enemy_field(_db_id, "weapon1", _second_weapon_code);
        DBManager::I().update_enemy_field(_db_id, "weapon2", "");
        _second_weapon_code = "";
    }
}

unsigned int Enemy::get_bullets_count (const std::string & code)
{
    return DBManager::I().get_enemy_field <unsigned int> (_db_id, "bullets");
}

void Enemy::remove_one_bullet (const std::string & code)
{
    unsigned int count = get_bullets_count(code);
    if (count > 0) DBManager::I().update_enemy_field(_db_id, "bullets", NumberToString(--count));
}

void Enemy::remove_self_from_db ()
{
    DBManager::I().delete_enemy_from_map(_db_id);
}
