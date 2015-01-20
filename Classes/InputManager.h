#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "cocos2d.h"
USING_NS_CC;

// Возможности для учета прокрутки колеса мыши рабочие, но закомментированы.

struct NAPR
{
    bool left, right, up, down;
    
    bool is_true () const;
    Vec2 get_direction ();
};

class InputManager
{
public:
    static InputManager & I ();
    ~InputManager () = default;
    
    // На случай, если недостаточно просто отслеживать параметры и использовать их 
    // в методах запланированного обратного вызова (Scheduler), можно назначить свои
    // методы обратного вызова, которые будут выполнены после внесения изменений 
    // в члены класса, хранящие данные о входящих сигналах. 
    std::function < void (int) > mouse_down_callback = nullptr;
    std::function < void (int) > mouse_up_callback = nullptr;
    std::function < void (float, float) > mouse_move_callback = nullptr;
//    std::function < void (float, float) > mouse_scroll_callback = nullptr;
    std::function < void (EventKeyboard::KeyCode) > key_press_callback = nullptr;
    std::function < void (EventKeyboard::KeyCode) > key_release_callback = nullptr;
    
private:
    InputManager ();
    InputManager (const InputManager & other) = delete;
    InputManager (const InputManager && other) = delete;
    InputManager & operator= (const InputManager & other) = delete;
    InputManager & operator= (const InputManager && other) = delete;
    
    enum class MouseKeyKode {LEFT, RIGHT, UNDEFINED};
    
    EventListenerMouse * _mouseListener = nullptr;
    EventListenerKeyboard * _keyboardListener = nullptr;
    Point mouse_pos {0.0f, 0.0f};
//    std::pair <float, float> scroll_xy {0.0f, 0.0f};
    
    // Было бы логично поставить bool вместо char, но это если не учесть, 
    // что кнопка с одним и тем же кодом может быть нажата дважды
    // (наирмер, ctrl). И вдруг будет подключена вторая мышь? :)
    std::map <MouseKeyKode, unsigned char> mouse_keys;
    std::map <EventKeyboard::KeyCode, unsigned char> keyboard_keys;
    
    // Инициализируем или обнуляем все отслеживаемые значения.
    void set_zero_values ();
    // Преобразуем int в MouseKeyKode.
    MouseKeyKode MKK_from_int (int ikey);
    
    void onMouseDown (Event* event);
    void onMouseUp (Event* event);
    void onMouseMove (Event* event);
//    void onMouseScroll (Event* event);
    
    void onKeyPressed (EventKeyboard::KeyCode key_kode, Event * event);
    void onKeyReleased (EventKeyboard::KeyCode key_kode, Event * event);
    
public:
    // disp должен принадлежать ноде node.
    // Одновременно может быть привязка только к одной ноде.
    void link_with_node (EventDispatcher * disp, Node * node);
    // disp должен быть тот же, что и при вызове link_with_node.
    // После отвязки от ноды все нажатые кнопки сбрасываются, и их нужно отпустить и заново нажать.
    // Так же обнуляются все обратные вызовы.
    void unlink_with_node (EventDispatcher * disp);
    // В процессе перехода сообщения о входящих сигналах кокосом не отправляются.
    
    // Возвращается битовая маска нажатых клавиш. 1000 - w, 100 - a, 10 - s, 1 - d.
    unsigned char check_wasd ();
    // Возвращается битовая маска нажатых стрелок. 1000 - left, 100 - right, 10 - up, 1 - down.
    unsigned char check_arrows_lrud ();
    // Возвращает true, если нажата клавиша ctrl.
    bool is_ctrl ();
    // Возвращает true, если нажата левая кнопка мыши.
    bool is_mouse_left_key ();
    // Возвращает true, если нажата правая кнопка мыши.
    bool is_mouse_right_key ();
    // Возвращает текущие координаты мыши.
    Point get_mouse_pos ();
    
    // Методы, заканчивающиеся на _play_rules, проверяют входящие сигналы с учетом зависимостей:
    // если нажата Ctrl, то НАПР и ЛКМ игнорируются; 
    // если нажата НАПР, то ЛКМ игнорируется;
    // если нажата хотя бы одна стрелка, то нажатие WASD игронируется.
    NAPR check_NAPR_play_rules ();
    bool is_ctrl_play_rules ();
    bool is_mouse_left_key_play_rules ();
    
    // Возвращается, сколько было прокручено с начала отслеживания или с момента последнего запроса.
//    float get_scroll_x ();
//    float get_scroll_y ();
};

#endif // INPUTMANAGER_H
