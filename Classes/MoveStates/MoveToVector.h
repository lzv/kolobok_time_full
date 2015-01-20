#ifndef MOVETOVECTOR_H
#define MOVETOVECTOR_H

#include "MoveState.h"

class MoveToVector : public MoveState
{
public:
    virtual void check_input (MapActor * ch) override;
    virtual void update_character (float dt, MapActor * ch) override;
    
    MoveToVector (const Vec2 & direction);
    virtual ~MoveToVector () = default;
    
protected:
    Vec2 _direction; // Нормализованный вектор направления.
};

#endif // MOVETOVECTOR_H
