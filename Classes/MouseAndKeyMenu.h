#ifndef MOUSEANDKEYMENU_H
#define MOUSEANDKEYMENU_H

/*
 * Данный класс похож на класс кокоса Menu, но отличается в деталях. 
 * Работа с меню идет не через прикосновения, а через отслеживание мыши и клавиатуры.
 * Один пункт меню всегда выбран, его можно активировать нажатием на Enter.
 * Так же его можно менять нажатиями клавиш стрелок вверх и вниз (если выравнивание вертикально),
 * или же клавиш стрелок влево и вправо (когда выравнивание горизонтально). 
 * При наведении мыши пункт выделяется сразу, а не после нажатия левой кнопки.
 * Пункт активируется после нажатия левой кнопки мыши, а не после отпускания.
 * Так же, после активации выделение пункта не снимается.
 * 
 * Предполагается, что после формирования, меню не меняется - не меняется активность пунктов, 
 * пункты не добавляются и не удаляются. Формирование заканчивается вызовом одного из методов
 * выравнивания, после чего начинает действовать графическое выделение выбранного пункта.
 * 
 * Доработка: 
 * Решил добавить второй режим. Первый режим, как описано выше и действует по умолчанию.
 * Второй режим - действует только мышь, при наведении мыши пункт выделяется, при уходе мыши выделение снимается. 
 * Режим устанавливается при создании меню и в дальйнейшем не меняется.
 */

#include "cocos2d.h"
NS_CC_BEGIN
#include "Macros.h"

class MouseAndKeyMenu : public Node
{
public:
    enum class ModeAlwaysSelected {YES, NO};
    
    static MouseAndKeyMenu * create (bool need_green_check, ModeAlwaysSelected mode = ModeAlwaysSelected::YES);
    
    virtual void addChild (Node * child) override;
    virtual void addChild (Node * child, int zOrder) override;
    virtual void addChild (Node * child, int zOrder, int tag) override;
    virtual void addChild (Node * child, int zOrder, const std::string & name) override;
    virtual void removeChild (Node * child, bool cleanup) override;
    
    void alignItemsVertically ();
    void alignItemsVerticallyWithPadding (float padding);
    void alignItemsHorizontally ();
    void alignItemsHorizontallyWithPadding (float padding);
    
protected:
    MouseAndKeyMenu (bool need_green_check, ModeAlwaysSelected mode);
    virtual ~MouseAndKeyMenu ();
    
    enum class Alignment {VERTICALLY, HORIZONTALLY, NONE};
    
    bool init ();
    void set_selected_to_first_enabled ();
    int get_index_item_under_mouse (float absolute_x, float absolute_y);
    // Выбор циклический.
    void select_next ();
    void select_prev ();
    void add_green_check (MenuItem * child);
    
    void onMouseDown (Event* event);
    void onMouseMove (Event* event);
    void onKeyPressed (EventKeyboard::KeyCode key_kode, Event * event);
    
    EventListenerMouse * _mouseListener;
    EventListenerKeyboard * _keyboardListener;
    Alignment _current_aligment;
    ModeAlwaysSelected _always_selected_mode;
    int _selected_index;
    bool _need_green_check;
    
private:
    DELETE_COPY_AND_MOVE(MouseAndKeyMenu);
};

NS_CC_END

#endif // MOUSEANDKEYMENU_H
