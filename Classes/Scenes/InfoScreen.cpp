#include "InfoScreen.h"
#include "StartScreen.h"
#include "../MouseAndKeyMenu.h"
#include "../NormalMenuItemLabel.h"
#include "../InputManager.h"

bool InfoScreen::init ()
{
    if (!GameScreen::init()) return false;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto return_menu = MouseAndKeyMenu::create(false, MouseAndKeyMenu::ModeAlwaysSelected::NO);
    return_menu->addChild(NormalMenuItemLabel::create(
        Label::createWithSystemFont("<-- вернуться в главное меню", "Arial", 36),
        std::bind(& InfoScreen::onExitScreen, this)
    ));
    return_menu->alignItemsVerticallyWithPadding(10.0f);
    return_menu->setAnchorPoint(Vec2(0.0f, 1.0f));
    return_menu->setPosition(56.0f, visibleSize.height);
    addChild(return_menu, 0);
    
    std::string text("Однажды Дед и Бабка решили испечь хлеба. Наделали из теста колобков, но печь маленькая, "
                     "и помещается только один. Поставили они в печь первого колобка, и пока ждали, остальные "
                     "колобки и поразбежались! Заплакали Дед и Бабка. А тут и первый колобок испекся. Выходит "
                     "он из печи и говорит им: \"Не плачь Дед, не плачь Бабка! Верну я вам ваших колобков. С добрым "
                     "словом и пистолетом я смогу их убедить!\" Взял колобок кинжал и пошел искать остальных колобков. "
                     "Кто не спрятался, он не виноват!");
    auto l = Label::createWithSystemFont(text, "Arial", 34);
    l->setAnchorPoint(Vec2(0.5f, 0.5f));
    l->setPosition(visibleSize.width / 2.0f, (visibleSize.height / 2.0f) - 28.0f);
    l->setHeight(visibleSize.height - 76.0f);
    l->setWidth(visibleSize.width - 20.0f);
    l->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
    l->setColor(Color3B {0x88, 0xff, 0x88});
    addChild(l, 1);
    
    std::string text2("Всем привет! Это еще не полноценная игра, а скорей заготовка для нее, основа. Для полноценной игры "
                     "нужно добавить уровней, контента, да и нарисовать все покрасивей. Сделал же я эту игру для того, "
                     "что бы получить опыт в самостоятельном выполнении проекта такой сложности, и моя цель достигнута. "
                     "Если же кто-нибудь заинтересуется в развитии этой заготовки до полноценной игры, свяжитесь со мной: "
                     "vic2_@mail.ru, сайт lzv.name.");
    l = Label::createWithSystemFont(text2, "Arial", 34);
    l->setAnchorPoint(Vec2(0.5f, 1.0f));
    l->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f + 180.0f);
    l->setHeight(visibleSize.height / 2.0f);
    l->setWidth(visibleSize.width - 20.0f);
    l->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
    addChild(l, 2);
    
    std::string text3("Используйте кнопки стрелок или WASD, что бы напрямую управлять персонажем. Кликните по карте, что бы "
                      "персонаж пошел к той точке. Кликните на врага, что бы преследовать его и атаковать, как только "
                      "позволит радиус действия оружия. Если в время движения понадобится остановиться на месте, нажмите ctrl. "
                      "Нажмите и удерживайте правую кнопку мыши, что бы атаковать в указанном направлении. Нажмите C, что бы "
                      "открыть окно персонажа и/или повысить характеристики.");
    l = Label::createWithSystemFont(text3, "Arial", 34);
    l->setAnchorPoint(Vec2(0.5f, 1.0f));
    l->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f - 35.0f);
    l->setHeight(visibleSize.height / 2.0f);
    l->setWidth(visibleSize.width - 20.0f);
    l->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
    l->setColor(Color3B {0x88, 0xff, 0x88});
    addChild(l, 2);
    
    return true;
}

void InfoScreen::onExitScreen ()
{
    Director::getInstance()->replaceScene(TransitionSlideInL::create(0.5f, StartScreen::create()));
}

void InfoScreen::onEnterTransitionDidFinish ()
{
    GameScreen::onEnterTransitionDidFinish();
    
    InputManager::I().key_press_callback = [this](EventKeyboard::KeyCode code){
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) onExitScreen();
    };
}
