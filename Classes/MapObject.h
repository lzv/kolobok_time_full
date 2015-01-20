#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include "cocos2d.h"
#include "ObjectsParams.h"
USING_NS_CC;
#include "Macros.h"
#include "BoolBitMap.h"

class Character;

// ---------------------     MapObject    -----------------------------------

class MapObject : public Node
{
public:
    virtual void check_touch (Character * ch) = 0;
    virtual bool is_intersect_with_point (const Point & point) = 0;
    virtual void add_self_into_db (unsigned int map_number);
    virtual void delete_self_from_db ();
    virtual void set_db_id (long int db_id) final;
    virtual long int get_db_id () const final;
    
protected:
    virtual ~MapObject () = default;
    MapObject () = default;
    
    std::string _code{""};
    long int _db_id = -1;  // -1 у объектов, которых нет в БД (постоянные объекты на карте).
    bool _is_done = false;
    
    virtual bool init (const std::string & code);
    virtual void run_touch_action (Character *) = 0;
    
    DELETE_COPY_AND_MOVE(MapObject);
};

// ---------------------     MapVisibleObject    -----------------------------------

class MapVisibleObject : public MapObject
{
public:
    virtual void check_touch (Character * ch) override;
    virtual bool is_intersect_with_point (const Point & point) override;
    
protected:
    virtual ~MapVisibleObject () = default;
    MapVisibleObject () = default;
    
    Sprite * _sprite = nullptr;
    BoolBitMap _bit_map;
    int _width = 0;
    int _height = 0;
    
    virtual bool init (const std::string & code, const std::string & sprite_file_name);
    virtual void run_touch_action (Character *) override = 0;
    
    DELETE_COPY_AND_MOVE(MapVisibleObject);
};

// ---------------------     MapInvisibleObject    -----------------------------------

class MapInvisibleObject : public MapObject
{
public:
    virtual void check_touch (Character * ch) override;
    virtual bool is_intersect_with_point (const Point & point) override;
    
protected:
    virtual ~MapInvisibleObject () = default;
    MapInvisibleObject () = default;
    
    int _width = 0;
    int _height = 0;
    
    virtual bool init (const std::string & code, int width, int height);
    virtual void run_touch_action (Character *) override = 0;
    
    DELETE_COPY_AND_MOVE(MapInvisibleObject);
};

// ---------------------     MapBullets    -----------------------------------

class MapBullets : public MapVisibleObject
{
public:
    static MapBullets * create (const BulletParams & params, unsigned int count);
    
    virtual void add_self_into_db (unsigned int map_number) override;
    virtual void delete_self_from_db () override;
    
protected:
    virtual ~MapBullets () = default;
    MapBullets () = default;
    
    unsigned int _count = 0;
    std::string _name_for_user{""};
    
    virtual bool init (const BulletParams & params, unsigned int count);
    virtual void run_touch_action (Character * ch) override;
    
    DELETE_COPY_AND_MOVE(MapBullets);
};

// ---------------------     MapWeapon    -----------------------------------

class MapWeapon : public MapVisibleObject
{
public:
    static MapWeapon * create (const WeaponParams & params);
    
    virtual void add_self_into_db (unsigned int map_number) override;
    virtual void delete_self_from_db () override;
    
protected:
    virtual ~MapWeapon () = default;
    MapWeapon () = default;
    
    std::string _name_for_user{""};
    
    virtual bool init (const WeaponParams & params);
    virtual void run_touch_action (Character * ch) override;
    
    DELETE_COPY_AND_MOVE(MapWeapon);
};

// ---------------------     Transitions    -----------------------------------

class MapTransition
{
protected:
    virtual ~MapTransition () = default;
    MapTransition (unsigned int target_map_number, float target_pos_x, float target_pos_y);
    
    virtual void run_touch_action (Character * ch);
    
    unsigned int _target_map_number;
    float _target_pos_x, _target_pos_y;
};

// -------------------------

class MapVisibleTransition : public MapVisibleObject, public MapTransition
{
public:
    static MapVisibleTransition * create (const TransitionOnMapParams & params);
    
protected:
    virtual ~MapVisibleTransition () = default;
    MapVisibleTransition (const TransitionOnMapParams & params);
    
    std::string _name_for_user{""};
    
    virtual bool init (const TransitionOnMapParams & params);
    virtual void run_touch_action (Character * ch) override;
    
    DELETE_COPY_AND_MOVE(MapVisibleTransition);
};

// -------------------------

class MapInvisibleTransition : public MapInvisibleObject, public MapTransition
{
public:
    static MapInvisibleTransition * create (const TransitionOnMapParams & params);
    
protected:
    virtual ~MapInvisibleTransition () = default;
    MapInvisibleTransition (const TransitionOnMapParams & params);
    
    std::string _name_for_user{""};
    
    virtual bool init (const TransitionOnMapParams & params);
    virtual void run_touch_action (Character * ch) override;
    
    DELETE_COPY_AND_MOVE(MapInvisibleTransition);
};

// -------------------------

MapObject * create_map_transition (const TransitionOnMapParams & params);

// ---------------------     MapHealthPotion    -----------------------------------

class MapHealthPotion : public MapVisibleObject
{
public:
    static MapHealthPotion * create (const HealthPotionMapParams & params);
    
    virtual void add_self_into_db (unsigned int map_number) override;
    virtual void delete_self_from_db () override;
    
protected:
    virtual ~MapHealthPotion () = default;
    MapHealthPotion () = default;
    
    std::string _name_for_user{""};
    unsigned int _hp_count;
    
    virtual bool init (const HealthPotionMapParams & params);
    virtual void run_touch_action (Character * ch) override;
    
    DELETE_COPY_AND_MOVE(MapHealthPotion);
};

#endif // MAPOBJECT_H
