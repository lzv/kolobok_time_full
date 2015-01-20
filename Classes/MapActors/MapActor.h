#ifndef MAPACTOR_H
#define MAPACTOR_H

#include "cocos2d.h"
USING_NS_CC;
#include "../Macros.h"
#include "../BoolBitMap.h"

class CharWeapon;
class PlayScreen;
class MoveState;
class AttackState;

class MapActor : public Node
{
public:
    virtual bool init (
        const std::string & sprite_file_name, 
        const std::string & current_weapon_code,
        bool need_hits_line
    );
    
    const float _base_speed;          // Базовая скорость, пикселей в секунду.
    PlayScreen * const _main_screen;  // Для доступа к другим объектам.
    BoolBitMap _bit_map;
    unsigned int _bit_map_width = 0;
    unsigned int _bit_map_height = 0;
    CharWeapon * _current_weapon = nullptr;
    
    virtual void replace_move_state (MoveState * new_state);
    virtual void replace_attack_state (AttackState * new_state);
    virtual bool is_intersect_with_point (const Point & point);
    virtual Point get_left_bottom_bit_map_point ();
    virtual void set_position (float x, float y);
    virtual void set_position (const Vec2 & position);
    virtual void check_move_and_attack (float delta_time);
    virtual void set_rotate_angle (float angle);  // Угол в градусах. 
    virtual float get_rotate_angle ();            // Угол в градусах. 
    virtual void on_target_dead ();
    virtual void set_popup_string (const std::string & text, const Color3B & color);
    virtual unsigned int get_xp_price ();         // Сколько XP получит персонаж за победу.
    virtual unsigned int get_current_hp () = 0;
    virtual unsigned int get_max_hp () = 0;
    virtual unsigned int get_strength () = 0;
    virtual unsigned int get_accuracy () = 0;
    virtual unsigned int get_bullets_count (const std::string & code) = 0;
    virtual void remove_one_bullet (const std::string & code) = 0;
    virtual void add_hp (unsigned int count) = 0;
    virtual void add_damage (unsigned int damage) = 0;
    
protected:
    MapActor (float base_speed, PlayScreen * screen);
    virtual ~MapActor ();
    
    Sprite * _body = nullptr;
    Sprite * _hits_line = nullptr;
    MoveState * _move_state = nullptr;
    AttackState * _attack_state = nullptr;
    
    virtual void set_new_weapon (const std::string & code);
    virtual void update_hits_line ();
    
    DELETE_COPY_AND_MOVE(MapActor);
};

#endif // MAPACTOR_H
