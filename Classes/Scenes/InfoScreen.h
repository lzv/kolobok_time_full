#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include "GameScreen.h"

class InfoScreen : public GameScreen 
{
public:
    CREATE_FUNC (InfoScreen);
    bool init ();
    
protected:
    virtual ~InfoScreen () = default;
    InfoScreen () = default;
    
    void onExitScreen ();
    
    virtual void onEnterTransitionDidFinish () override;
    
    DELETE_COPY_AND_MOVE(InfoScreen);
};

#endif // INFOSCREEN_H
