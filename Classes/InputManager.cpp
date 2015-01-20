#include "InputManager.h"

bool NAPR::is_true () const {return left || right || up || down;}

Vec2 NAPR::get_direction ()
{
    Vec2 result(0.0f, 0.0f);
    if (left) result.x -= 1.0f;
    if (right) result.x += 1.0f;
    if (up) result.y += 1.0f;
    if (down) result.y -= 1.0f;
    result.normalize();
    return result;
}

InputManager & InputManager::I ()
{
    static InputManager obj;
    return obj;
}

InputManager::InputManager ()
{
    set_zero_values();
}

void InputManager::set_zero_values ()
{
    mouse_keys[MouseKeyKode::LEFT] = 0;
    mouse_keys[MouseKeyKode::RIGHT] = 0;
    
    keyboard_keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_UP_ARROW] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_DOWN_ARROW] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_S] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_W] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_D] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_A] = 0;
    keyboard_keys[EventKeyboard::KeyCode::KEY_CTRL] = 0;
}

InputManager::MouseKeyKode InputManager::MKK_from_int (int ikey)
{
    MouseKeyKode result;
    switch (ikey)
    {
        case 0:  result = MouseKeyKode::LEFT;      break;
        case 1:  result = MouseKeyKode::RIGHT;     break;
        default: result = MouseKeyKode::UNDEFINED;
    }
    return result;
}

void InputManager::link_with_node (EventDispatcher * disp, Node * node)
{
    if (_mouseListener == nullptr && _keyboardListener == nullptr)
    {
        _mouseListener = EventListenerMouse::create();
        _mouseListener->onMouseMove = CC_CALLBACK_1(InputManager::onMouseMove, this);
        _mouseListener->onMouseUp = CC_CALLBACK_1(InputManager::onMouseUp, this);
        _mouseListener->onMouseDown = CC_CALLBACK_1(InputManager::onMouseDown, this);
//        _mouseListener->onMouseScroll = CC_CALLBACK_1(InputManager::onMouseScroll, this);
        disp->addEventListenerWithSceneGraphPriority(_mouseListener, node);
        
        _keyboardListener = EventListenerKeyboard::create();
        _keyboardListener->onKeyPressed = CC_CALLBACK_2(InputManager::onKeyPressed, this);
        _keyboardListener->onKeyReleased = CC_CALLBACK_2(InputManager::onKeyReleased, this);
        disp->addEventListenerWithSceneGraphPriority(_keyboardListener, node);
    }
}

void InputManager::unlink_with_node (EventDispatcher * disp)
{
    if (_mouseListener != nullptr && _keyboardListener != nullptr)
    {
        disp->removeEventListener(_mouseListener);
        disp->removeEventListener(_keyboardListener);
        _mouseListener = nullptr;
        _keyboardListener = nullptr;
        
        mouse_down_callback = nullptr;
        mouse_up_callback = nullptr;
        mouse_move_callback = nullptr;
//        mouse_scroll_callback = nullptr;
        key_press_callback = nullptr;
        key_release_callback = nullptr;
        
        set_zero_values();
    }
}

void InputManager::onMouseDown (Event *event)
{
    EventMouse * e = dynamic_cast <EventMouse *> (event);
    if (e) {
        int button_number = e->getMouseButton();
        mouse_keys[MKK_from_int(button_number)]++;
        if (mouse_down_callback) mouse_down_callback(button_number);
        //log("Mouse down detected, key: %d", button_number);
    }
}

// Отпусканий может быть зарегистрировано больше, чем нажатий.
void InputManager::onMouseUp (Event *event)
{
    EventMouse * e = dynamic_cast <EventMouse *> (event);
    if (e) {
        int button_number = e->getMouseButton();
        MouseKeyKode key_code = MKK_from_int(button_number);
        if (mouse_keys[key_code] > 0) 
        {
            mouse_keys[key_code]--;
            if (mouse_up_callback) mouse_up_callback(button_number);
        }
        //log("Mouse up detected, key: %d", button_number);
    }
}

void InputManager::onMouseMove (Event *event)
{
    EventMouse * e = dynamic_cast <EventMouse *> (event);
    if (e) {
        mouse_pos.x = e->getCursorX();
        mouse_pos.y = e->getCursorY();
        if (mouse_move_callback) mouse_move_callback(mouse_pos.x, mouse_pos.y);
        //log("Mouse move, position: x %f, y %f", mouse_pos.x, mouse_pos.y);
    }
}
/*
void InputManager::onMouseScroll (Event *event)
{
    EventMouse * e = dynamic_cast <EventMouse *> (event);
    if (e) {
        scroll_xy.first += e->getScrollX();
        scroll_xy.second += e->getScrollY();
        if (mouse_scroll_callback) mouse_scroll_callback(scroll_xy.first, scroll_xy.second);
        //log("Mouse scroll: x %f, y %f", scroll_xy.first, scroll_xy.second);
    }
}
*/
void InputManager::onKeyPressed (EventKeyboard::KeyCode key_kode, Event * event)
{
    //log("Нажата клавиша: %d", key_kode);
    keyboard_keys[key_kode]++;
    if (key_press_callback) key_press_callback(key_kode);
}

// Отпусканий может быть зарегистрировано больше, чем нажатий.
void InputManager::onKeyReleased (EventKeyboard::KeyCode key_kode, Event * event)
{
    //log("Отпущена клавиша: %d", key_kode);
    if (keyboard_keys[key_kode] > 0)
    {
        keyboard_keys[key_kode]--;
        if (key_release_callback) key_release_callback(key_kode);
    }
}

unsigned char InputManager::check_wasd () 
{
    unsigned char result = 0;
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_W] > 0) result |= static_cast <unsigned char> (8); // 1000
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_A] > 0) result |= static_cast <unsigned char> (4); // 100
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_S] > 0) result |= static_cast <unsigned char> (2); // 10
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_D] > 0) result |= static_cast <unsigned char> (1); // 1
    return result;
}

unsigned char InputManager::check_arrows_lrud ()
{
    unsigned char result = 0;
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW] > 0) result |= static_cast <unsigned char> (8); // 1000
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] > 0) result |= static_cast <unsigned char> (4); // 100
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_UP_ARROW] > 0) result |= static_cast <unsigned char> (2); // 10
    if (keyboard_keys[EventKeyboard::KeyCode::KEY_DOWN_ARROW] > 0) result |= static_cast <unsigned char> (1); // 1
    return result;
}

bool InputManager::is_ctrl ()
{
    return keyboard_keys[EventKeyboard::KeyCode::KEY_CTRL] > 0;
}

bool InputManager::is_mouse_left_key ()
{
    return mouse_keys[MouseKeyKode::LEFT] > 0;
}

bool InputManager::is_mouse_right_key ()
{
    return mouse_keys[MouseKeyKode::RIGHT] > 0;
}

Point InputManager::get_mouse_pos ()
{
    return mouse_pos;
}

NAPR InputManager::check_NAPR_play_rules ()
{
    NAPR result {false, false, false, false};
    if (!is_ctrl())
    {
        unsigned char lrud = check_arrows_lrud();
        if (lrud)
        {
            result.left = lrud & static_cast <unsigned char> (8);
            result.right = lrud & static_cast <unsigned char> (4);
            result.up = lrud & static_cast <unsigned char> (2);
            result.down = lrud & static_cast <unsigned char> (1);
        }
        else {
            unsigned char wasd = check_wasd();
            if (wasd)
            {
                result.up = wasd & static_cast <unsigned char> (8);
                result.left = wasd & static_cast <unsigned char> (4);
                result.down = wasd & static_cast <unsigned char> (2);
                result.right = wasd & static_cast <unsigned char> (1);
            }
        }
    }
    return result;
}

bool InputManager::is_ctrl_play_rules ()
{
    return is_ctrl();
}

bool InputManager::is_mouse_left_key_play_rules ()
{
    return (is_ctrl() || check_wasd() || check_arrows_lrud()) ? false : is_mouse_left_key();
}

/*
float InputManager::get_scroll_x ()
{
    float result = scroll_xy.first;
    scroll_xy.first = 0.0f;
    return result;
}

float InputManager::get_scroll_y ()
{
    float result = scroll_xy.second;
    scroll_xy.second = 0.0f;
    return result;
}
*/