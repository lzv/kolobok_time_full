#ifndef DEADSCREEN_H
#define DEADSCREEN_H

#include "GameScreen.h"

class DeadScreen : public GameScreen 
{
public:
    CREATE_FUNC (DeadScreen);
    bool init ();
    
protected:
    virtual ~DeadScreen () = default;
    DeadScreen () = default;
    
    void onLoadGame ();
    void onExitToStartScreen ();
    void onExitGame ();
    
    DELETE_COPY_AND_MOVE(DeadScreen);
};

#endif // DEADSCREEN_H
