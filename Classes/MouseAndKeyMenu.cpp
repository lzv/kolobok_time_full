#include "MouseAndKeyMenu.h"

NS_CC_BEGIN

const float defaultPadding = 5.0f;

MouseAndKeyMenu * MouseAndKeyMenu::create (bool need_green_check, ModeAlwaysSelected mode)
{
    MouseAndKeyMenu * p = new MouseAndKeyMenu(need_green_check, mode);
    if (p && p->init())
    {
        p->autorelease();
        return p;
    }
    else {
        delete p;
        return nullptr;
    }
}

MouseAndKeyMenu::MouseAndKeyMenu (bool need_green_check, ModeAlwaysSelected mode)
    : _mouseListener(nullptr), _keyboardListener(nullptr), 
    _current_aligment(Alignment::NONE), _always_selected_mode(mode), _selected_index(-1), 
    _need_green_check(need_green_check)
{}

MouseAndKeyMenu::~MouseAndKeyMenu ()
{
    _eventDispatcher->removeEventListener(_mouseListener);
    _eventDispatcher->removeEventListener(_keyboardListener);
}

bool MouseAndKeyMenu::init ()
{
    if (!Node::init()) return false;
    
    _mouseListener = EventListenerMouse::create();
    _mouseListener->onMouseMove = CC_CALLBACK_1(MouseAndKeyMenu::onMouseMove, this);
    _mouseListener->onMouseDown = CC_CALLBACK_1(MouseAndKeyMenu::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);
    
    _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = CC_CALLBACK_2(MouseAndKeyMenu::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);
    
    return true;
}

void MouseAndKeyMenu::alignItemsVertically ()
{
    alignItemsVerticallyWithPadding(defaultPadding);
}

void MouseAndKeyMenu::alignItemsVerticallyWithPadding (float padding)
{
    _current_aligment = Alignment::VERTICALLY;
    
    float height = -padding;
    float max_width = 0.0f;
    for (const auto & child : _children)
    {
        height += child->getContentSize().height + padding;
        if (max_width < child->getContentSize().width) max_width = child->getContentSize().width;
    }
    float x = max_width / 2.0f;
    float y = height;
    
    for (const auto & child : _children)
    {
        child->setPosition(x, y - child->getContentSize().height / 2.0f);
        child->setAnchorPoint(Vec2(0.5f, 0.5f));
        y -= child->getContentSize().height + padding;
    }
    
    setContentSize(Size(max_width, height));
    if (_always_selected_mode == ModeAlwaysSelected::YES) set_selected_to_first_enabled();
}

void MouseAndKeyMenu::alignItemsHorizontally ()
{
    alignItemsHorizontallyWithPadding(defaultPadding);
}

void MouseAndKeyMenu::alignItemsHorizontallyWithPadding (float padding)
{
    _current_aligment = Alignment::HORIZONTALLY;
    
    float width = -padding;
    float max_height = 0.0f;
    for (const auto & child : _children)
    {
        width += child->getContentSize().width + padding;
        if (max_height < child->getContentSize().height) max_height = child->getContentSize().height;
    }
    float x = 0.0f;
    float y = max_height / 2.0f;
    
    for (const auto & child : _children)
    {
        child->setPosition(x + child->getContentSize().width / 2.0f, y);
        child->setAnchorPoint(Vec2(0.5f, 0.5f));
        x += child->getContentSize().width + padding;
    }
    
    setContentSize(Size(width, max_height));
    if (_always_selected_mode == ModeAlwaysSelected::YES) set_selected_to_first_enabled();
}

// Установить выделение у первого включенного пункта.
void MouseAndKeyMenu::set_selected_to_first_enabled ()
{
    _selected_index = -1;
    int children_count = _children.size();
    for (int i = 0; i < children_count; ++i)
    {
        MenuItem * child = dynamic_cast <MenuItem *> (_children.at(i));
        if (child && child->isEnabled())
        {
            _selected_index = i;
            child->selected();
            break;
        }
    }
}

int MouseAndKeyMenu::get_index_item_under_mouse (float absolute_x, float absolute_y)
{
    Vec2 local_coords = convertToNodeSpace(Vec2(absolute_x, absolute_y));
    // Сначала проверим выбранный пункт, так как он увеличен в размерах и может частично перекрыть соседние.
    if (_selected_index >= 0 && _children.at(_selected_index)->getBoundingBox().containsPoint(local_coords))
        return _selected_index;
    // Теперь проверяем остальные.
    int children_count = _children.size();
    for (int i = 0; i < children_count; ++i)
        if (i != _selected_index)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(i));
            if (child->isEnabled() && child->getBoundingBox().containsPoint(local_coords))
                return i;
        }
    // Нужных пунктов не найдено.
    return -1;
}

void MouseAndKeyMenu::onMouseDown (Event *event)
{
    EventMouse * e = dynamic_cast <EventMouse *> (event);
    if (e)
    {
        int item_under_mouse = get_index_item_under_mouse(e->getCursorX(), e->getCursorY());
        if (item_under_mouse >= 0)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(item_under_mouse));
            if (child)
            {
                add_green_check(child);
                runAction(Sequence::create(
                    DelayTime::create(0.0001f), /* что бы галочки успели нарисоваться */
                    CallFunc::create([child](){child->activate();}),
                    nullptr
                ));
            }
        }
    }
}

void MouseAndKeyMenu::add_green_check (MenuItem * child)
{
    if (_need_green_check)
    {
        auto csize = child->getContentSize();
        
        Sprite * check1 = Sprite::create("green_check.png");
        check1->setAnchorPoint(Vec2(1.0f, 0.5f));
        check1->setPosition(-10.0f, csize.height / 2.0f);
        child->addChild(check1);
        
        Sprite * check2 = Sprite::create("green_check.png");
        check2->setAnchorPoint(Vec2(0.0f, 0.5f));
        check2->setPosition(csize.width + 10.0f, csize.height / 2.0f);
        child->addChild(check2);
    }
}

void MouseAndKeyMenu::onMouseMove (Event *event)
{
    EventMouse * e = dynamic_cast <EventMouse *> (event);
    if (e)
    {
        int item_under_mouse = get_index_item_under_mouse(e->getCursorX(), e->getCursorY());
        if (_always_selected_mode == ModeAlwaysSelected::YES)
        {
            if (item_under_mouse >= 0 && item_under_mouse != _selected_index)
            {
                if (_selected_index >= 0)
                {
                    MenuItem * old_child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
                    if (old_child) old_child->unselected();
                }
                _selected_index = item_under_mouse;
                MenuItem * new_child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
                if (new_child) new_child->selected();
            }
        }
        else {
            if (item_under_mouse != _selected_index)
            {
                if (_selected_index >= 0)
                {
                    MenuItem * old_child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
                    if (old_child) old_child->unselected();
                    _selected_index = -1;
                }
                if (item_under_mouse >= 0)
                {
                    _selected_index = item_under_mouse;
                    MenuItem * new_child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
                    if (new_child) new_child->selected();
                }
            }
        }
    }
}

void MouseAndKeyMenu::onKeyPressed (EventKeyboard::KeyCode key_kode, Event * event)
{
    if (_always_selected_mode == ModeAlwaysSelected::YES && _selected_index >= 0)
    {
        if (key_kode == EventKeyboard::KeyCode::KEY_KP_ENTER)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
            if (child)
            {
                add_green_check(child);
                runAction(Sequence::create(
                    DelayTime::create(0.0001f), /* что бы галочки успели нарисоваться */
                    CallFunc::create([child](){child->activate();}),
                    nullptr
                ));
            }
        }
        else if (_current_aligment == Alignment::VERTICALLY 
            && (key_kode == EventKeyboard::KeyCode::KEY_UP_ARROW || key_kode == EventKeyboard::KeyCode::KEY_DOWN_ARROW))
        {
            if (key_kode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) select_next();
            else select_prev();
        }
        else if (_current_aligment == Alignment::HORIZONTALLY 
            && (key_kode == EventKeyboard::KeyCode::KEY_LEFT_ARROW || key_kode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
        {
            if (key_kode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) select_next();
            else select_prev();
        }
    }
}

void MouseAndKeyMenu::select_next ()
{
    if (_selected_index >= 0)
    {
        int children_count = _children.size();
        int result = -1;
        for (int i = _selected_index + 1; i < children_count; ++i)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(i));
            if (child && child->isEnabled())
            {
                result = i;
                child->selected();
                break;
            }
        }
        if (result < 0)
            for (int i = 0; i < _selected_index; ++i)
            {
                MenuItem * child = dynamic_cast <MenuItem *> (_children.at(i));
                if (child && child->isEnabled())
                {
                    result = i;
                    child->selected();
                    break;
                }
            }
        if (result >= 0)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
            if (child) child->unselected();
            _selected_index = result;
        }
    }
}

void MouseAndKeyMenu::select_prev ()
{
    if (_selected_index >= 0)
    {
        int result = -1;
        for (int i = _selected_index - 1; i >= 0; --i)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(i));
            if (child && child->isEnabled())
            {
                result = i;
                child->selected();
                break;
            }
        }
        if (result < 0)
            for (int i = _children.size() - 1; i > _selected_index; --i)
            {
                MenuItem * child = dynamic_cast <MenuItem *> (_children.at(i));
                if (child && child->isEnabled())
                {
                    result = i;
                    child->selected();
                    break;
                }
            }
        if (result >= 0)
        {
            MenuItem * child = dynamic_cast <MenuItem *> (_children.at(_selected_index));
            if (child) child->unselected();
            _selected_index = result;
        }
    }
}

void MouseAndKeyMenu::addChild (Node * child)
{
    CCASSERT( dynamic_cast <MenuItem*> (child) != nullptr, "MouseAndKeyMenu only supports MenuItem objects as children");
    Node::addChild(child);
}

void MouseAndKeyMenu::addChild (Node * child, int zOrder)
{
    CCASSERT( dynamic_cast <MenuItem*> (child) != nullptr, "MouseAndKeyMenu only supports MenuItem objects as children");
    Node::addChild(child, zOrder);
}

void MouseAndKeyMenu::addChild (Node * child, int zOrder, int tag)
{
    CCASSERT( dynamic_cast <MenuItem*> (child) != nullptr, "MouseAndKeyMenu only supports MenuItem objects as children");
    Node::addChild(child, zOrder, tag);
}

void MouseAndKeyMenu::addChild (Node * child, int zOrder, const std::string & name)
{
    CCASSERT( dynamic_cast <MenuItem*> (child) != nullptr, "MouseAndKeyMenu only supports MenuItem objects as children");
    Node::addChild(child, zOrder, name);
}

void MouseAndKeyMenu::removeChild (Node * child, bool cleanup)
{
    MenuItem * menuItem = dynamic_cast <MenuItem*> (child);
    CCASSERT(menuItem != nullptr, "MouseAndKeyMenu only supports MenuItem objects as children");
    Node::removeChild(child, cleanup);
}

NS_CC_END