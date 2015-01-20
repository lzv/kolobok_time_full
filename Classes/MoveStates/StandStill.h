#ifndef STANDSTILL_H
#define STANDSTILL_H

#include "MoveState.h"

class StandStill : public MoveState
{
public:
    virtual void check_input (MapActor * ch) override;
    virtual void update_character (float dt, MapActor * ch) override;
    
    StandStill () = default;
    virtual ~StandStill () = default;
};

#endif // STANDSTILL_H
