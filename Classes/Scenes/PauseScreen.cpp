#include "PauseScreen.h"
#include "StartScreen.h"
#include "PlayScreen.h"
#include "../MouseAndKeyMenu.h"
#include "../NormalMenuItemLabel.h"
#include "../DBManager.h"
#include "../InputManager.h"

bool PauseScreen::init ()
{
    if (!GameScreen::init()) return false;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto title = Label::createWithSystemFont("Пауза", "Arial", 50);
    title->setAnchorPoint(Vec2(0.5f, 1.0f));
    title->setPosition(visibleSize.width / 2.0f, visibleSize.height - 150.0f);
    addChild(title, 0);
    
    _menu = create_menu();
    addChild(_menu);
    
    return true;
}

void PauseScreen::onEnterTransitionDidFinish ()
{
    GameScreen::onEnterTransitionDidFinish();
    
    InputManager::I().key_press_callback = [this](EventKeyboard::KeyCode code){
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) onReturnToGame();
    };
}

void PauseScreen::onReturnToGame ()
{
    Director::getInstance()->popScene();
}

void PauseScreen::onSaveGame ()
{
    DBManager::I().save();
    _menu->pause();
    runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this](){
            removeChild(_menu, true);
            _menu = create_menu();
            addChild(_menu);
        }),
        nullptr
    ));
}

void PauseScreen::onLoadGame ()
{
    DBManager::I().load();
    Director::getInstance()->popScene();
    Director::getInstance()->replaceScene(PlayScreen::create());
}

void PauseScreen::onExitToStartScreen ()
{
    Director::getInstance()->popScene();
    Director::getInstance()->replaceScene(StartScreen::create());
}

void PauseScreen::onExitGame ()
{
    Director::getInstance()->end();
}

MouseAndKeyMenu * PauseScreen::create_menu ()
{
    auto menu = MouseAndKeyMenu::create(true);
    
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Вернуться к игре", "Arial", 40),
        std::bind(& PauseScreen::onReturnToGame, this)
    ));
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Сохранить игру", "Arial", 40),
        std::bind(& PauseScreen::onSaveGame, this)
    ));
    auto nmit = NormalMenuItemLabel::create(
        Label::createWithSystemFont("Загрузить сохраненную игру", "Arial", 40),
        std::bind(& PauseScreen::onLoadGame, this)
    );
    nmit->setEnabled(DBManager::I().is_save_exist());
    menu->addChild(nmit);
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Выйти в главное меню", "Arial", 40),
        std::bind(& PauseScreen::onExitToStartScreen, this)
    ));
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Выйти из игры", "Arial", 40),
        std::bind(& PauseScreen::onExitGame, this)
    ));
    
    menu->alignItemsVerticallyWithPadding(10.0f);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    menu->setAnchorPoint(Vec2(0.5f, 0.5f));
    menu->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f);
    
    return menu;
}
