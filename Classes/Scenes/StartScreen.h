#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "GameScreen.h"

class StartScreen : public GameScreen 
{
public:
    CREATE_FUNC (StartScreen);
    bool init ();
    
protected:
    virtual ~StartScreen () = default;
    StartScreen () = default;
    
    void onNewGame ();
    void onContinueGame ();
    void onShowInfo ();
    void onExitGame ();
    
    virtual void onEnterTransitionDidFinish () override;
    
    DELETE_COPY_AND_MOVE(StartScreen);
};

#endif // STARTSCREEN_H
