#ifndef ENEMY_H
#define ENEMY_H

#include "MapActor.h"
#include "../DBManager.h"

// при изменении местоположения записывать его в БД
// так же записывать трату пуль и смену оружия, и угол поворота

class Enemy : public MapActor
{
public:
    static Enemy * create (const EnemyDBParams & params, PlayScreen * screen);
    bool init (const EnemyDBParams & params);
    
    virtual void set_position (float x, float y) override;
    virtual void set_position (const Vec2 & position) override;
    virtual unsigned int get_current_hp () override;
    virtual unsigned int get_max_hp () override;
    virtual void add_hp (unsigned int count) override;
    virtual void add_damage (unsigned int damage) override;
    virtual unsigned int get_strength () override;
    virtual unsigned int get_accuracy () override;
    virtual unsigned int get_bullets_count (const std::string & code) override;
    void change_weapon ();
    virtual void remove_one_bullet (const std::string & code) override;
    void remove_self_from_db ();
    
protected:
    Enemy (PlayScreen * screen);
    virtual ~Enemy ();
    
    unsigned int _db_id = 0;
    std::string _second_weapon_code = "";
    std::string _name_for_user = "";
    unsigned int _strength = 0;
    unsigned int _accuracy = 0;
    
    DELETE_COPY_AND_MOVE(Enemy);
};

#endif // ENEMY_H
