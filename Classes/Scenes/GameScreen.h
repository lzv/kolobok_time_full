#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include "cocos2d.h"
USING_NS_CC;
#include "../Macros.h"

class GameScreen : public Scene
{
public:
    bool init ();
    
protected:
    virtual ~GameScreen () = default;
    GameScreen () = default;
    
    // Здесь в классах-наследниках навешивать методы обратного вызова в объект InputManager.
    virtual void onExitTransitionDidStart ();   // Вызывается первый у уходящей сцены.
    virtual void onEnterTransitionDidFinish (); // Вызывается второй у приходящей сцены.
    
    DELETE_COPY_AND_MOVE(GameScreen);
};

#endif // GAMESCREEN_H
