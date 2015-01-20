#include "MapActor.h"
#include "../Tools.h"
#include "../CharWeapon.h"
#include "../Scenes/PlayScreen.h"
#include "../MoveStates/MoveState.h"
#include "../AttackStates/AttackState.h"

MapActor::MapActor (float base_speed, PlayScreen * screen)
    : Node(), _base_speed(base_speed), _main_screen(screen)
{}

MapActor::~MapActor ()
{
    if (_move_state) delete _move_state;
    if (_attack_state) delete _attack_state;
}

bool MapActor::init (const std::string & sprite_file_name, const std::string & current_weapon_code, bool need_hits_line)
{
    if (!Node::init()) return false;
    
    _body = Sprite::create(sprite_file_name);
    auto csize = _body->getContentSize();
    _body->setAnchorPoint(Vec2(0.5f, 0.5f));
    _body->setPosition(csize.width / 2.0f, csize.height / 2.0f);
    addChild(_body);
    setContentSize(csize);
    setAnchorPoint(Vec2(0.5f, 0.5f));
    
    init_bool_bit_map_from_image(_bit_map, sprite_file_name);
    _bit_map_width = _bit_map._width;
    _bit_map_height = _bit_map._height;
    CCASSERT(_bit_map_width > 0 && _bit_map_height > 0, "MapActor - bit map size is 0");
    
    _current_weapon = create_weapon(current_weapon_code);
    _body->addChild(_current_weapon);
    
    if (need_hits_line)
    {
        Sprite * red_line = Sprite::create("red_pixel.png");
        red_line->setAnchorPoint(Vec2(0.5f, 0.0f));
        red_line->setPosition(csize.width / 2.0f, csize.height + 2.0f);
        red_line->setScaleX(50.0f);
        red_line->setScaleY(5.0f);
        addChild(red_line, 0);
        
        _hits_line = Sprite::create("green_pixel.png");
        _hits_line->setAnchorPoint(Vec2(0.0f, 0.0f));
        _hits_line->setPosition(0.0f, 0.0f);
        _hits_line->setScaleX(static_cast <float> (get_current_hp()) / get_max_hp());
        red_line->addChild(_hits_line, 0);
    }
    
    return true;
}

void MapActor::replace_move_state (MoveState * new_state)
{
    if (_move_state) delete _move_state;
    _move_state = new_state;
}

void MapActor::replace_attack_state (AttackState * new_state)
{
    if (_attack_state) delete _attack_state;
    _attack_state = new_state;
}

bool MapActor::is_intersect_with_point (const Point & point)
{
    // Объекты MapActor имеют форму круга с anchor point в центре. Поэтому вращение можно игнорировать.
    return is_intersect_bitmap_and_point(get_left_bottom_bit_map_point(), _bit_map, point);
}

Point MapActor::get_left_bottom_bit_map_point ()
{
    return Point{_position.x - _contentSize.width / 2.0f, _position.y - _contentSize.height / 2.0f};
}

void MapActor::set_position (float x, float y)
{
    setPosition(x, y);
}

void MapActor::set_position (const Vec2 & position)
{
    setPosition(position);
}

void MapActor::check_move_and_attack (float delta_time)
{
    if (_move_state)
    {
        _move_state->check_input(this);
        _move_state->update_character(delta_time, this);
    }
    
    if (_attack_state)
    {
        _attack_state->check_input(this);
        _attack_state->actions(delta_time, this);
    }
}

void MapActor::set_new_weapon (const std::string & code)
{
    _body->removeChild(_current_weapon, true);
    _current_weapon = create_weapon(code);
    _body->addChild(_current_weapon);
}

void MapActor::set_rotate_angle (float angle)
{
    while (angle < 0.0f) angle += 360.0f;
    while (angle > 360.0f) angle -= 360.0f;
    if (angle > 90.0f && angle < 270.0f)
    {
        _body->setScaleX(-1.0f);
        _body->setRotation(angle - 180.0f);
    }
    else {
        _body->setScaleX(1.0f);
        _body->setRotation(angle);
    }
}

float MapActor::get_rotate_angle ()
{
    return _body->getRotation() + (_body->getScaleX() > 0.0f ? 0.0f : 180.0f);
}

void MapActor::on_target_dead ()
{
    if (_move_state) _move_state->on_target_dead(this);
    if (_attack_state) _attack_state->on_target_dead(this);
}

void MapActor::update_hits_line ()
{
    if (_hits_line) _hits_line->setScaleX(static_cast <float> (get_current_hp()) / get_max_hp());
}

void MapActor::set_popup_string (const std::string & text, const Color3B & color)
{
    auto label = Label::createWithSystemFont(text, "Arial", 26);
    label->setColor(color);
    label->setAnchorPoint(Vec2(0.5f, 0.0f));
    label->setPosition(_contentSize.width / 2.0f, _contentSize.height + 8.0f);
    addChild(label, 5);
    label->runAction(Sequence::create(
        Spawn::create(
            MoveBy::create(1.0f, Vec2(0.0f, 30.0f)),
            ScaleTo::create(1.0f, 0.5f),
            nullptr
        ),
        RemoveSelf::create(true),
        nullptr
    ));
}

unsigned int MapActor::get_xp_price ()
{
    return (get_strength() + get_accuracy()) * 200u;
}
