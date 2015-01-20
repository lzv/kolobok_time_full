#include "GameScreen.h"
#include "../InputManager.h"

bool GameScreen::init ()
{
    return Scene::init();
}

void GameScreen::onExitTransitionDidStart ()
{
    Scene::onExitTransitionDidStart();
    InputManager::I().unlink_with_node(_eventDispatcher);
}

void GameScreen::onEnterTransitionDidFinish ()
{
    Scene::onEnterTransitionDidFinish();
    InputManager::I().link_with_node(_eventDispatcher, this);
}
