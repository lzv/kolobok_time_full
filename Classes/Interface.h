#ifndef INTERFACE_H
#define INTERFACE_H

#include "cocos2d.h"
USING_NS_CC;
#include "Macros.h"

class Character;

class Interface
{
public:
    Interface () = default;
    ~Interface () = default;
    
    void init (Layer * layer, Character * character);
    void update ();
    void show_char_window ();
    void hide_char_window ();
    bool is_char_window_visible ();
    void on_mouse_click (const Point & click_window_point);
    
private:
    Layer * _layer = nullptr;
    Character * _character = nullptr;
    bool _initialized = false;
    Label * _p_bullets_count = nullptr;
    Label * _m_bullets_count = nullptr;
    Sprite * _green_line = nullptr;
    float _hp_line_scale = 200.0f;
    Label * _values_hp = nullptr;
    Sprite * _green_arrow_up = nullptr;
    
    bool _char_window_is_visible = false;
    Sprite * _char_window_background1 = nullptr;
    Sprite * _char_window_background2 = nullptr;
    std::map <std::string, Label *> _char_window_labels;
    Sprite * _strength_plus = nullptr;
    Sprite * _accuracy_plus = nullptr;
    
    DELETE_COPY_AND_MOVE(Interface);
};

#endif // INTERFACE_H
