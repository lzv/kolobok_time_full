#ifndef PAUSESCREEN_H
#define PAUSESCREEN_H

#include "GameScreen.h"

NS_CC_BEGIN
class MouseAndKeyMenu;
NS_CC_END

class PauseScreen : public GameScreen 
{
public:
    CREATE_FUNC (PauseScreen);
    bool init ();
    
protected:
    virtual ~PauseScreen () = default;
    PauseScreen () = default;
    
    MouseAndKeyMenu * _menu = nullptr;
    
    void onReturnToGame ();
    void onSaveGame ();
    void onLoadGame ();
    void onExitToStartScreen ();
    void onExitGame ();
    
    MouseAndKeyMenu * create_menu ();
    virtual void onEnterTransitionDidFinish () override;
    
    DELETE_COPY_AND_MOVE(PauseScreen);
};

#endif // PAUSESCREEN_H
