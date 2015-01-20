#ifndef WEAPON_H
#define WEAPON_H

#include "cocos2d.h"
#include "ObjectsParams.h"
USING_NS_CC;
#include "Macros.h"

class MapActor;

//-----------------

class CharWeapon : public Node
{
public:
    // Начало атаки, когда пользователь нажал кнопку или при преследовании противник оказался в радиусе действия оружия.
    virtual void start_attack (MapActor * attacker) = 0;
    // Окончание атаки, когда пользователь отпустил кнопку или противник умер.
    virtual void stop_attack ();
    // Возврат количества патронов для текущего оружия у переданного персонажа.
    virtual unsigned int bullets_count_at (MapActor * actor);
    
    const std::string _code;          // Кодовое имя объекта, идентификатор.
    WeaponType _type;                 // Тип оружия - ближнего боя или огнестрельное.
    const float _one_attack_time;     // Время в секундах, которое занимает одна атака.
    const unsigned int _base_damage;  // Базовый урон.
    const std::string _name_for_user; // Нормальное (не кодовое) имя объекта для игрока.
    const float _radius;              // Радиус действия оружия.
    const float _char_pos_x;          // Позиция на персонаже x.
    const float _char_pos_y;          // Позиция на персонаже y.
    
protected:
    virtual ~CharWeapon () = default;
    CharWeapon (const WeaponParams & params);
    
    virtual bool init (const WeaponParams & params);
    
    bool _is_attack_running = false;    // Идет ли атака в текущий момент.
    bool _is_attack_in_process = false; // Пока идет текущая атака, нельзя начинать следующую. 
    Sprite * _sprite = nullptr;         // Картинка оружия.
    
    DELETE_COPY_AND_MOVE(CharWeapon);
};

//-----------------

class MeleeWeapon : public CharWeapon
{
public:
    static MeleeWeapon * create (const WeaponParams & params);
    virtual void start_attack (MapActor * attacker) override;
    
protected:
    virtual ~MeleeWeapon() = default;
    MeleeWeapon (const WeaponParams & params);
    
    virtual bool init (const WeaponParams & params);
    void one_attack (MapActor *);
    
    DELETE_COPY_AND_MOVE(MeleeWeapon);
};

//-----------------

class FireWeapon : public CharWeapon
{
public:
    static FireWeapon * create (const WeaponParams & params);
    virtual void start_attack (MapActor * attacker) override;
    virtual unsigned int bullets_count_at (MapActor * actor) override;
    
    const std::string _bullet_code; // Код используемых патронов.
    
protected:
    virtual ~FireWeapon() = default;
    FireWeapon (const WeaponParams & params);
    
    Sprite * _flash = nullptr;
    
    virtual bool init (const WeaponParams & params);
    void one_attack (MapActor *, bool);
    
    DELETE_COPY_AND_MOVE(FireWeapon);
};

//-----------------

// Возвращается autorelease-объект.
CharWeapon * create_weapon (const std::string & code);

#endif // WEAPON_H
