#include "Character.h"
#include "../DBManager.h"
#include "../MoveStates/StandStill.h"
#include "../AttackStates/NoAttack.h"
#include "../Scenes/PlayScreen.h"

Character * Character::create (PlayScreen * screen)
{
    Character * p = new Character(screen);
    if (p && p->init())
    {
        p->autorelease();
        return p;
    }
    else {
        delete p;
        return nullptr;
    }
}

Character::Character (PlayScreen * screen) : MapActor(150.0f, screen) {}

bool Character::init ()
{
    std::string weapon = DBManager::I().get_character_value <std::string> ("weapon");
    if (!MapActor::init("kolobok.png", weapon, false)) return false;
    
    _move_state = new StandStill();
    _attack_state = new NoAttack();
    
    MapActor::set_rotate_angle(DBManager::I().get_character_value <float> ("rotate_angle"));
    
    unsigned int current_level = DBManager::I().get_character_value <unsigned int> ("level");
    for (unsigned int i = 1; i <= current_level; ++i) xp_to_next_level += i * 1000u;
    
    return true;
}

void Character::add_xp (unsigned int xp_count)
{
    unsigned int current_xp = DBManager::I().get_character_value <unsigned int> ("experience") + xp_count;
    DBManager::I().set_character_value("experience", current_xp);
    set_popup_string(std::string("+") + NumberToString(xp_count) + std::string(" XP"), Color3B{0x00, 0xff, 0x00});
    
    if (current_xp >= xp_to_next_level)
    {
        unsigned int level = get_level() + 1u;
        DBManager::I().set_character_value("level", level);
        xp_to_next_level += level * 1000u;
        
        unsigned int cp = get_free_cp() + 1u;
        DBManager::I().set_character_value("free_CP", cp);
        
        runAction(Sequence::create(
            DelayTime::create(0.25f),
            CallFunc::create([this](){set_popup_string("+1 level", Color3B{0x00, 0xff, 0x00});}),
            nullptr
        ));
        
        _main_screen->updateInterface();
    }
}

unsigned int Character::get_level ()
{
    return DBManager::I().get_character_value <unsigned int> ("level");
}

unsigned int Character::get_free_cp ()
{
    return DBManager::I().get_character_value <unsigned int> ("free_CP");
}

void Character::add_bullets (const std::string & code, unsigned int count)
{
    BulletParams params = get_bullet_params(code);
    unsigned int current_count = DBManager::I().get_character_value <unsigned int> (params.character_db_code);
    current_count += count;
    DBManager::I().set_character_value(params.character_db_code, current_count);
    _main_screen->updateInterface();
}

unsigned int Character::get_bullets_count (const std::string & code)
{
    return DBManager::I().get_character_value <unsigned int> (get_bullet_params(code).character_db_code);
}

void Character::add_hp (unsigned int count)
{
    int hits_damage = DBManager::I().get_character_value <int> ("hits_damage");
    int real_add_hp = std::min(hits_damage, static_cast <int> (count));
    hits_damage -= static_cast <int> (count);
    if (hits_damage < 0) hits_damage = 0;
    DBManager::I().set_character_value("hits_damage", hits_damage);
    _main_screen->updateInterface();
    set_popup_string(std::string("+") + NumberToString(real_add_hp), Color3B{0x00, 0xff, 0x00});
}

void Character::add_damage (unsigned int damage)
{
    unsigned int hits_damage = DBManager::I().get_character_value <unsigned int> ("hits_damage") + damage;
    DBManager::I().set_character_value("hits_damage", hits_damage);
    _main_screen->updateInterface();
    set_popup_string(std::string("-") + NumberToString(damage), Color3B{0xff, 0x00, 0x00});
}

unsigned int Character::get_current_hp ()
{
    return get_max_hp() - DBManager::I().get_character_value <unsigned int> ("hits_damage");
}

unsigned int Character::get_max_hp ()
{
    return 10 + 5 * get_strength();
}

void Character::set_position (float x, float y)
{
    setPosition(x, y);
    DBManager::I().set_character_value("pos_x", x);
    DBManager::I().set_character_value("pos_y", y);
}

void Character::set_position (const Vec2 & position)
{
    set_position(position.x, position.y);
}

void Character::set_rotate_angle (float angle)
{
    MapActor::set_rotate_angle(angle);
    DBManager::I().set_character_value("rotate_angle", angle);
}

void Character::set_new_weapon (const std::string & code)
{
    MapActor::set_new_weapon(code);
    DBManager::I().set_character_value("weapon", code);
}

unsigned int Character::get_strength ()
{
    return DBManager::I().get_character_value <unsigned int> ("strength");
}

unsigned int Character::get_accuracy ()
{
    return DBManager::I().get_character_value <unsigned int> ("accuracy");
}

void Character::add_one_strength ()
{
    unsigned int cp = get_free_cp();
    if (cp > 0u)
    {
        DBManager::I().set_character_value("strength", get_strength() + 1u);
        DBManager::I().set_character_value("free_CP", --cp);
    }
}

void Character::add_one_accuracy ()
{
    unsigned int cp = get_free_cp();
    if (cp > 0u)
    {
        DBManager::I().set_character_value("accuracy", get_accuracy() + 1u);
        DBManager::I().set_character_value("free_CP", --cp);
    }
}

void Character::remove_one_bullet (const std::string & code)
{
    unsigned int count = get_bullets_count(code);
    if (count > 0)
    {
        DBManager::I().set_character_value(get_bullet_params(code).character_db_code, --count);
        _main_screen->updateInterface();
    }
}
