#include "MapObject.h"
#include "MapActors/Character.h"
#include "Tools.h"
#include "DBManager.h"
#include "Scenes/PlayScreen.h"

// ---------------------     MapObject    -----------------------------------

bool MapObject::init (const std::string & code)
{
    if (!Node::init()) return false;
    _code = code;
    setAnchorPoint(Vec2(0.5f, 0.5f));
    return true;
}

void MapObject::set_db_id (long int db_id) {_db_id = db_id;}

long int MapObject::get_db_id () const {return _db_id;}

void MapObject::add_self_into_db (unsigned int map_number) {}

void MapObject::delete_self_from_db () {}

// ---------------------     MapVisibleObject    -----------------------------------

bool MapVisibleObject::init (const std::string & code, const std::string & sprite_file_name)
{
    if (!MapObject::init(code)) return false;
    
    _sprite = Sprite::create(sprite_file_name);
    init_bool_bit_map_from_image(_bit_map, sprite_file_name);
    
    bool is_ok = (_sprite != nullptr && _bit_map.is_init());
    if (is_ok)
    {
        _width = _bit_map._width;
        _height = _bit_map._height;
        
        _sprite->setAnchorPoint(Vec2(0.0f, 0.0f));
        addChild(_sprite);
        setContentSize(_sprite->getContentSize());
    }
    
    return is_ok;
}

void MapVisibleObject::check_touch (Character * ch)
{
    if (!_is_done && is_intersect_two_bitmaps (getBoundingBox().origin, _bit_map, ch->getBoundingBox().origin, ch->_bit_map))
        run_touch_action(ch);
}

bool MapVisibleObject::is_intersect_with_point (const Point & point)
{
    // Объекты MapObject не вращаются.
    return is_intersect_bitmap_and_point(getBoundingBox().origin, _bit_map, point);
}

// ---------------------     MapInvisibleObject    -----------------------------------

bool MapInvisibleObject::init (const std::string & code, int width, int height)
{
    if (!MapObject::init(code)) return false;
    _width = width;
    _height = height;
    setContentSize(Size(_width, _height));
    return true;
}

void MapInvisibleObject::check_touch (Character * ch)
{
    if (!_is_done && is_intersect_bitmap_and_rect(ch->getBoundingBox().origin, ch->_bit_map, getBoundingBox()))
        run_touch_action(ch);
}

bool MapInvisibleObject::is_intersect_with_point (const Point & point)
{
    return getBoundingBox().containsPoint(point);
}

// ---------------------     MapBullets    -----------------------------------

MapBullets * MapBullets::create (const BulletParams & params, unsigned int count)
{
    MapBullets * p = new MapBullets();
    if (p && p->init(params, count))
    {
        p->autorelease();
        return p;
    }
    else {
        delete p;
        return nullptr;
    }
}

bool MapBullets::init (const BulletParams & params, unsigned int count)
{
    if (!MapVisibleObject::init(params.code, params.img_file_name)) return false;
    _name_for_user = params.name_for_user;
    _count = count;
    return true;
}

void MapBullets::run_touch_action (Character * ch)
{
    _is_done = true;
    ch->add_bullets(_code, _count);
    ch->set_popup_string(std::string("+") + NumberToString(_count), Color3B{0x00, 0xff, 0x00});
    ch->_main_screen->deleteMapObject(this);
}

void MapBullets::add_self_into_db (unsigned int map_number)
{
    int x = static_cast <int> (getPositionX() + 0.5f);
    int y = static_cast <int> (getPositionY() + 0.5f);
    BulletsDBParams params {0, map_number, _code, x, y, _count};
    DBManager::I().add_bullet_on_map(params);
    _db_id = DBManager::I().get_last_insert_id();
}

void MapBullets::delete_self_from_db ()
{
    if (_db_id > 0) DBManager::I().delete_bullet_from_map(_db_id);
}

// ---------------------     MapWeapon    -----------------------------------

MapWeapon * MapWeapon::create (const WeaponParams & params)
{
    MapWeapon * p = new MapWeapon();
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

bool MapWeapon::init (const WeaponParams & params)
{
    if (!MapVisibleObject::init(params.code, params.img_file_name)) return false;
    _name_for_user = params.name_for_user;
    return true;
}

void MapWeapon::run_touch_action (Character * ch)
{
    _is_done = true;
    ch->set_new_weapon(_code);
    ch->_main_screen->deleteMapObject(this);
}

void MapWeapon::add_self_into_db (unsigned int map_number)
{
    int x = static_cast <int> (getPositionX() + 0.5f);
    int y = static_cast <int> (getPositionY() + 0.5f);
    WeaponsDBParams params {0, map_number, _code, x, y};
    DBManager::I().add_weapon_on_map(params);
    _db_id = DBManager::I().get_last_insert_id();
}

void MapWeapon::delete_self_from_db ()
{
    if (_db_id > 0) DBManager::I().delete_weapon_from_map(_db_id);
}

// ---------------------     Transitions    -----------------------------------

MapTransition::MapTransition (unsigned int target_map_number, float target_pos_x, float target_pos_y)
    : _target_map_number(target_map_number), _target_pos_x(target_pos_x), _target_pos_y(target_pos_y)
{}

void MapTransition::run_touch_action (Character * ch)
{
    ch->_main_screen->set_pause(true);
    ch->_main_screen->pause();
    DBManager::I().set_character_value("map_number", _target_map_number);
    DBManager::I().set_character_value("pos_x", _target_pos_x);
    DBManager::I().set_character_value("pos_y", _target_pos_y);
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, PlayScreen::create(), Color3B::BLACK));
}

// -------------------------

MapVisibleTransition * MapVisibleTransition::create (const TransitionOnMapParams & params)
{
    MapVisibleTransition * p = new MapVisibleTransition(params);
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

MapVisibleTransition::MapVisibleTransition (const TransitionOnMapParams & params)
    : MapVisibleObject(), MapTransition(params.target_map_number, params.target_pos_x, params.target_pos_y)
{}

bool MapVisibleTransition::init (const TransitionOnMapParams & params)
{
    if (params.type_code.length() == 0) return false;
    VisibleTransitionParams vtp = get_visible_transition_params(params.type_code);
    if (!MapVisibleObject::init(vtp.code, vtp.img_file_name)) return false;
    _name_for_user = params.name_for_user;
    setAnchorPoint(Vec2(params.ap_x, params.ap_y));
    
    if (vtp.anim_params.is_valid())
    {
        auto anim = create_animation (vtp.anim_params);
        if (anim) _sprite->runAction(RepeatForever::create(Animate::create(anim)));
    }
    
    return true;
}

void MapVisibleTransition::run_touch_action (Character * ch)
{
    _is_done = true;
    MapTransition::run_touch_action(ch);
}

// -------------------------

MapInvisibleTransition * MapInvisibleTransition::create (const TransitionOnMapParams & params)
{
    MapInvisibleTransition * p = new MapInvisibleTransition(params);
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

MapInvisibleTransition::MapInvisibleTransition (const TransitionOnMapParams & params)
    : MapInvisibleObject(), MapTransition(params.target_map_number, params.target_pos_x, params.target_pos_y)
{}

bool MapInvisibleTransition::init (const TransitionOnMapParams & params)
{
    if (params.type_code.length() > 0) return false;
    if (!MapInvisibleObject::init(params.type_code, params.width, params.height)) return false;
    _name_for_user = params.name_for_user;
    setAnchorPoint(Vec2(params.ap_x, params.ap_y));
    return true;
}

void MapInvisibleTransition::run_touch_action (Character * ch)
{
    _is_done = true;
    MapTransition::run_touch_action(ch);
}

// -------------------------

MapObject * create_map_transition (const TransitionOnMapParams & params)
{
    if (params.type_code.length() > 0) return MapVisibleTransition::create(params);
    else return MapInvisibleTransition::create(params);
}

// ---------------------     MapHealthPotion    -----------------------------------

MapHealthPotion * MapHealthPotion::create (const HealthPotionMapParams & params)
{
    MapHealthPotion * p = new MapHealthPotion();
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

bool MapHealthPotion::init (const HealthPotionMapParams & params)
{
    if (!MapVisibleObject::init(params.code, params.img_file_name)) return false;
    _name_for_user = params.name_for_user;
    _hp_count = params.hp_count;
    return true;
}

void MapHealthPotion::add_self_into_db (unsigned int map_number)
{
    int x = static_cast <int> (getPositionX() + 0.5f);
    int y = static_cast <int> (getPositionY() + 0.5f);
    SimpleObjectDBParams params {0, map_number, SimpleObjectType::HEALTH_POTION, _code, x, y};
    DBManager::I().add_simple_object_on_map(params);
    _db_id = DBManager::I().get_last_insert_id();
}

void MapHealthPotion::delete_self_from_db ()
{
    if (_db_id > 0) DBManager::I().delete_simple_object_from_map(_db_id);
}

void MapHealthPotion::run_touch_action (Character * ch)
{
    _is_done = true;
    ch->add_hp(_hp_count);
    ch->_main_screen->deleteMapObject(this);
}
