#include "StartScreen.h"
#include "InfoScreen.h"
#include "PlayScreen.h"
#include "../MouseAndKeyMenu.h"
#include "../NormalMenuItemLabel.h"
#include "../DBManager.h"
#include "../InputManager.h"

bool StartScreen::init ()
{
    if (!GameScreen::init()) return false;
    
    auto menu = MouseAndKeyMenu::create(true);
    
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Новая игра", "Arial", 40),
        std::bind(& StartScreen::onNewGame, this)
    ));
    auto nmit = NormalMenuItemLabel::create(
        Label::createWithSystemFont("Продолжить игру", "Arial", 40),
        std::bind(& StartScreen::onContinueGame, this)
    );
    nmit->setEnabled(DBManager::I().is_save_exist());
    menu->addChild(nmit);
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Информация", "Arial", 40),
        std::bind(& StartScreen::onShowInfo, this)
    ));
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Выход", "Arial", 40),
        std::bind(& StartScreen::onExitGame, this)
    ));
    
    menu->alignItemsVerticallyWithPadding(10.0f);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    menu->setAnchorPoint(Vec2(0.5f, 0.5f));
    menu->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f);
    
    addChild(menu);
    
    return true;
}

void StartScreen::onNewGame ()
{
    DBManager::I().init_default_values();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, PlayScreen::create(), Color3B::BLACK));
}

void StartScreen::onContinueGame ()
{
    if (DBManager::I().is_save_exist())
    {
        DBManager::I().load();
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, PlayScreen::create(), Color3B::BLACK));
    }
}

void StartScreen::onShowInfo ()
{
    Director::getInstance()->replaceScene(TransitionSlideInR::create(0.5f, InfoScreen::create()));
}

void StartScreen::onExitGame ()
{
    Director::getInstance()->end();
}

void StartScreen::onEnterTransitionDidFinish ()
{
    GameScreen::onEnterTransitionDidFinish();
    
    InputManager::I().key_press_callback = [this](EventKeyboard::KeyCode code){
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) onExitGame();
    };
}
