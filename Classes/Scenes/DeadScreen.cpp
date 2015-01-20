#include "DeadScreen.h"
#include "PlayScreen.h"
#include "StartScreen.h"
#include "../DBManager.h"
#include "../MouseAndKeyMenu.h"
#include "../NormalMenuItemLabel.h"

bool DeadScreen::init ()
{
    if (!GameScreen::init()) return false;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto title = Label::createWithSystemFont("Ваш персонаж мертв", "Arial", 50);
    title->setAnchorPoint(Vec2(0.5f, 1.0f));
    title->setPosition(visibleSize.width / 2.0f, visibleSize.height - 150.0f);
    addChild(title, 0);
    
    auto menu = MouseAndKeyMenu::create(true);
    
    auto nmit = NormalMenuItemLabel::create(
        Label::createWithSystemFont("Загрузить сохраненную игру", "Arial", 40),
        std::bind(& DeadScreen::onLoadGame, this)
    );
    nmit->setEnabled(DBManager::I().is_save_exist());
    menu->addChild(nmit);
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Выйти в главное меню", "Arial", 40),
        std::bind(& DeadScreen::onExitToStartScreen, this)
    ));
    menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("Выйти из игры", "Arial", 40),
        std::bind(& DeadScreen::onExitGame, this)
    ));
    
    menu->alignItemsVerticallyWithPadding(10.0f);
    menu->setAnchorPoint(Vec2(0.5f, 0.5f));
    menu->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f);
    
    addChild(menu);
    
    return true;
}

void DeadScreen::onLoadGame ()
{
    DBManager::I().load();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, PlayScreen::create(), Color3B::BLACK));
}

void DeadScreen::onExitToStartScreen ()
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, StartScreen::create(), Color3B::BLACK));
}

void DeadScreen::onExitGame ()
{
    Director::getInstance()->end();
}
