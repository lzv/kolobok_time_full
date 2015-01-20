#ifndef CHARACTER_H
#define CHARACTER_H

#include "MapActor.h"

class Character : public MapActor
{
public:
    static Character * create (PlayScreen * screen);
    bool init ();
    
    void add_bullets (const std::string & code, unsigned int count);
    unsigned int get_bullets_count (const std::string & code) override;
    virtual void add_hp (unsigned int count) override;
    virtual void add_damage (unsigned int damage) override;
    virtual unsigned int get_current_hp () override;
    virtual unsigned int get_max_hp () override;
    virtual void set_position (float x, float y) override;
    virtual void set_position (const Vec2 & position) override;
    virtual void set_rotate_angle (float angle) override;
    virtual void set_new_weapon (const std::string & code) override;
    virtual unsigned int get_strength () override;
    virtual unsigned int get_accuracy () override;
    virtual void remove_one_bullet (const std::string & code) override;
    void add_xp (unsigned int xp_count);
    void add_one_strength ();
    void add_one_accuracy ();
    unsigned int get_level ();
    unsigned int get_free_cp ();
    
protected:
    Character (PlayScreen * screen);
    virtual ~Character () = default;
    
    unsigned int xp_to_next_level = 0;
    
    DELETE_COPY_AND_MOVE(Character);
};

#endif // CHARACTER_H
