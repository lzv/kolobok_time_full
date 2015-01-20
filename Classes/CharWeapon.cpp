#include "CharWeapon.h"
#include "MapActors/MapActor.h"
#include "Scenes/PlayScreen.h"

//--------------------- CharWeapon -------------------------------

CharWeapon::CharWeapon (const WeaponParams & params)
    : _code{params.code},
    _type{params.type},
    _one_attack_time(params.one_attack_time > 0.0f ? params.one_attack_time : 1.0f),
    _base_damage (params.base_damage > 0 ? params.base_damage : 1),
    _name_for_user(params.name_for_user),
    _radius(params.radius > 0.0f ? params.radius : 0.0f),
    _char_pos_x(params.char_pos_x), _char_pos_y(params.char_pos_y)
{}

bool CharWeapon::init (const WeaponParams & params)
{
    if (!Node::init()) return false;
    
    _sprite = Sprite::create(params.img_file_name);
    bool result = _sprite != nullptr;
    if (result) {
        addChild(_sprite, 0);
        _sprite->setAnchorPoint(Vec2(0.0f, 0.0f));
        setContentSize(_sprite->getContentSize());
    }
    
    setPosition(_char_pos_x, _char_pos_y);
    
    return result;
}

void CharWeapon::stop_attack ()
{
    _is_attack_running = false;
}

unsigned int CharWeapon::bullets_count_at (MapActor *)
{
    return 0;
}

//--------------------- MeleeWeapon -------------------------------

MeleeWeapon::MeleeWeapon (const WeaponParams & params) : CharWeapon(params) {}

MeleeWeapon * MeleeWeapon::create (const WeaponParams & params)
{
    MeleeWeapon * pRet = new MeleeWeapon(params);
    if (pRet && pRet->init(params))
    {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        return nullptr;
    }
}

bool MeleeWeapon::init (const WeaponParams & params)
{
    if (!CharWeapon::init(params)) return false;
    setAnchorPoint(Vec2(0.5f, 0.0f));
    return true;
}

void MeleeWeapon::start_attack (MapActor * actor)
{
    _is_attack_running = true;
    if (!_is_attack_in_process) one_attack(actor);
}

void MeleeWeapon::one_attack (MapActor * actor)
{
    _is_attack_in_process = true;
    float move_time = (_one_attack_time / 7.0f) * 2.0f;
    float wait_time = (_one_attack_time / 7.0f) * 3.0f;
    runAction(Sequence::create(
        Spawn::create(
            MoveTo::create(move_time, Point{50.0f, 25.0f}),
            RotateBy::create(move_time, 90.0f),
            nullptr
        ),
        CallFunc::create([actor](){actor->_main_screen->on_attack(actor);}),
        DelayTime::create(wait_time),
        Spawn::create(
            MoveTo::create(move_time, getPosition()),
            RotateBy::create(move_time, -90.0f),
            nullptr
        ),
        CallFunc::create([this, actor](){
            _is_attack_in_process = false;
            if (_is_attack_running) one_attack(actor);
        }),
        nullptr
    ));
}

//--------------------- FireWeapon -------------------------------

FireWeapon::FireWeapon (const WeaponParams & params)
    : CharWeapon(params), _bullet_code{params.bullet_code}
{}

FireWeapon * FireWeapon::create (const WeaponParams & params)
{
    FireWeapon * pRet = new FireWeapon(params);
    if (pRet && pRet->init(params))
    {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        return nullptr;
    }
}

bool FireWeapon::init (const WeaponParams & params)
{
    if (!CharWeapon::init(params)) return false;
    setAnchorPoint(Vec2(0.0f, 1.0f));
    
    _flash = Sprite::create("flash.png");
    if (!_flash) return false;
    _flash->setAnchorPoint(Vec2(0.0f, 0.5f));
    _flash->setPosition(params.flash_pos_x, params.flash_pos_y);
    _flash->setVisible(false);
    addChild(_flash, 1);
    
    return true;
}

void FireWeapon::start_attack (MapActor * actor)
{
    _is_attack_running = true;
    if (!_is_attack_in_process) one_attack(actor, true);
}

void FireWeapon::one_attack (MapActor * actor, bool first)
{
    if (actor->get_bullets_count(_bullet_code) > 0)
    {
        _is_attack_in_process = true;
        if (first) setPosition(getPosition() + Point{5.0f, 8.0f});
        runAction(Spawn::create(
            Sequence::create(
                TargetedAction::create(_flash, Show::create()),
                DelayTime::create(0.03f),
                TargetedAction::create(_flash, Hide::create()),
                nullptr
            ),
            Sequence::create(
                CallFunc::create([this, actor](){
                    actor->remove_one_bullet(_bullet_code);
                    actor->_main_screen->on_attack(actor);
                }),
                DelayTime::create(_one_attack_time),
                CallFunc::create([this, actor](){
                    _is_attack_in_process = false;
                    if (_is_attack_running) one_attack(actor, false);
                    else setPosition(_char_pos_x, _char_pos_y);
                }),
                nullptr
            ),
            nullptr
        ));
    }
    else {
        _is_attack_running = false;
        setPosition(_char_pos_x, _char_pos_y);
    }
}

unsigned int FireWeapon::bullets_count_at (MapActor * actor)
{
    return actor->get_bullets_count(_bullet_code);
}

//--------------------- create_weapon -------------------------------

CharWeapon * create_weapon (const std::string & code)
{
    CharWeapon * result = nullptr;
    WeaponParams params = get_weapon_params(code);
    if (params.type == WeaponType::MELEE) result = MeleeWeapon::create(params);
    else if (params.type == WeaponType::FIRE) result = FireWeapon::create(params);
    return result;
}
