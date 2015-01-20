#include "Interface.h"
#include "MapActors/Character.h"
#include "Tools.h"
#include "CharWeapon.h"

void Interface::init (Layer * layer, Character * character)
{
    if (!_initialized && layer && character)
    {
        _layer = layer;
        _character = character;
        
        auto csize = _layer->getContentSize();
        
        auto pb_sp = Sprite::create("objects/pistol_bullet.png");
        pb_sp->setPosition(csize.width - 100.0f, csize.height - 30.0f);
        pb_sp->setAnchorPoint(Vec2(1.0f, 0.5f));
        layer->addChild(pb_sp, 0);
        
        auto mb_sp = Sprite::create("objects/machine_bullet.png");
        mb_sp->setPosition(csize.width - 100.0f, csize.height - 80.0f);
        mb_sp->setAnchorPoint(Vec2(1.0f, 0.5f));
        layer->addChild(mb_sp, 0);
        
        unsigned int pustol_bullets_count = _character->get_bullets_count("pistol_bullet");
        _p_bullets_count = Label::createWithSystemFont(NumberToString(pustol_bullets_count), "Arial", 30);
        _p_bullets_count->setAnchorPoint(Vec2(0.0f, 0.5f));
        _p_bullets_count->setPosition(csize.width - 85.0f, csize.height - 30.0f);
        layer->addChild(_p_bullets_count, 0);
        
        unsigned int mashine_bullets_count = _character->get_bullets_count("machine_bullet");
        _m_bullets_count = Label::createWithSystemFont(NumberToString(mashine_bullets_count), "Arial", 30);
        _m_bullets_count->setAnchorPoint(Vec2(0.0f, 0.5f));
        _m_bullets_count->setPosition(csize.width - 85.0f, csize.height - 80.0f);
        layer->addChild(_m_bullets_count, 0);
        
        Point hp_line_pos{60, csize.height - 20.0f};
        
        auto hp_label = Label::createWithSystemFont("HP", "Arial", 30);
        hp_label->setAnchorPoint(Vec2(1.0f, 0.5f));
        hp_label->setPosition(hp_line_pos.x - 10.0f, hp_line_pos.y);
        layer->addChild(hp_label, 0);
        
        auto sp = Sprite::create("red_pixel.png");
        sp->setScaleX(_hp_line_scale);
        sp->setScaleY(10.0f);
        sp->setAnchorPoint(Vec2(0.0f, 0.5f));
        sp->setPosition(hp_line_pos);
        layer->addChild(sp, 0);
        
        unsigned int current_hp = _character->get_current_hp();
        unsigned int max_hp = _character->get_max_hp();
        
        _green_line = Sprite::create("green_pixel.png");
        _green_line->setScaleX(_hp_line_scale * (static_cast <float> (current_hp) / static_cast <float> (max_hp)));
        _green_line->setScaleY(10.0f);
        _green_line->setAnchorPoint(Vec2(0.0f, 0.5f));
        _green_line->setPosition(hp_line_pos);
        layer->addChild(_green_line, 1);
        
        _values_hp = Label::createWithSystemFont(NumberToString(current_hp) + " / " + NumberToString(max_hp), "Arial", 30);
        _values_hp->setAnchorPoint(Vec2(0.0f, 0.5f));
        _values_hp->setPosition(hp_line_pos.x + _hp_line_scale + 10.0f, hp_line_pos.y);
        layer->addChild(_values_hp, 0);
        
        _green_arrow_up = Sprite::create("level_up_0.png");
        _green_arrow_up->setAnchorPoint(Vec2(0.0f, 0.5f));
        _green_arrow_up->setPosition(_values_hp->getPosition() + Vec2(_values_hp->getContentSize().width + 15.0f, 0.0f));
        layer->addChild(_green_arrow_up, 0);
        
        unsigned int char_cp = character->get_free_cp();
        if (char_cp > 0u)
        {
            auto animation = create_animation("level_up_%d.png", 2, 0.5f, false, 20);
            _green_arrow_up->runAction(RepeatForever::create(Animate::create(animation)));
        }
        else {
            _green_arrow_up->setVisible(false);
        }
        
        Size back_size {400.0f, 200.0f};
        Point back_pos {150.0f, csize.height - 50.0f};
        float vline_x = back_pos.x + back_size.width * 0.5f;
        
        _char_window_background1 = Sprite::create("white_pixel.png");
        _char_window_background1->setScaleX(back_size.width);
        _char_window_background1->setScaleY(back_size.height);
        _char_window_background1->setAnchorPoint(Vec2(0.0f, 1.0f));
        _char_window_background1->setPosition(back_pos);
        layer->addChild(_char_window_background1, 0);
        
        _char_window_background2 = Sprite::create("gray_pixel.png");
        _char_window_background2->setScaleX(back_size.width - 6.0f);
        _char_window_background2->setScaleY(back_size.height - 6.0f);
        _char_window_background2->setAnchorPoint(Vec2(0.0f, 1.0f));
        _char_window_background2->setPosition(back_pos + Point(3.0f, -3.0f));
        layer->addChild(_char_window_background2, 1);
        
        _char_window_labels["title"] = Label::createWithSystemFont("Персонаж", "Arial", 26);
        _char_window_labels["title"]->setAnchorPoint(Vec2(0.5f, 0.0f));
        _char_window_labels["title"]->setPosition(back_pos + Point(back_size.width / 2.0f, -35.0f));
        
        _char_window_labels["level_title"] = Label::createWithSystemFont("Уровень:", "Arial", 24);
        _char_window_labels["level_title"]->setAnchorPoint(Vec2(1.0f, 0.5f));
        _char_window_labels["level_title"]->setPosition(vline_x, back_pos.y - 55.0f);
        
        _char_window_labels["level_value"] = Label::createWithSystemFont(NumberToString(_character->get_level()), "Arial", 24);
        _char_window_labels["level_value"]->setAnchorPoint(Vec2(0.0f, 0.5f));
        _char_window_labels["level_value"]->setPosition(vline_x + 15.0f, back_pos.y - 55.0f);
        
        _char_window_labels["strength_title"] = Label::createWithSystemFont("Сила:", "Arial", 24);
        _char_window_labels["strength_title"]->setAnchorPoint(Vec2(1.0f, 0.5f));
        _char_window_labels["strength_title"]->setPosition(vline_x, back_pos.y - 90.0f);
        
        _char_window_labels["strength_value"] = Label::createWithSystemFont(NumberToString(_character->get_strength()), "Arial", 24);
        _char_window_labels["strength_value"]->setAnchorPoint(Vec2(0.0f, 0.5f));
        _char_window_labels["strength_value"]->setPosition(vline_x + 15.0f, back_pos.y - 90.0f);
        
        _char_window_labels["accuracy_title"] = Label::createWithSystemFont("Точность:", "Arial", 24);
        _char_window_labels["accuracy_title"]->setAnchorPoint(Vec2(1.0f, 0.5f));
        _char_window_labels["accuracy_title"]->setPosition(vline_x, back_pos.y - 113.0f);
        
        _char_window_labels["accuracy_value"] = Label::createWithSystemFont(NumberToString(_character->get_accuracy()), "Arial", 24);
        _char_window_labels["accuracy_value"]->setAnchorPoint(Vec2(0.0f, 0.5f));
        _char_window_labels["accuracy_value"]->setPosition(vline_x + 15.0f, back_pos.y - 113.0f);
        
        _char_window_labels["cp_title"] = Label::createWithSystemFont("Осталось CP:", "Arial", 24);
        _char_window_labels["cp_title"]->setAnchorPoint(Vec2(1.0f, 0.5f));
        _char_window_labels["cp_title"]->setPosition(vline_x + 30.0f, back_pos.y - 145.0f);
        
        _char_window_labels["cp_value"] = Label::createWithSystemFont(NumberToString(_character->get_free_cp()), "Arial", 24);
        _char_window_labels["cp_value"]->setAnchorPoint(Vec2(0.0f, 0.5f));
        _char_window_labels["cp_value"]->setPosition(vline_x + 45.0f, back_pos.y - 145.0f);
        
        _char_window_labels["weapon_title"] = Label::createWithSystemFont("Текущее оружие:", "Arial", 24);
        _char_window_labels["weapon_title"]->setAnchorPoint(Vec2(1.0f, 0.5f));
        _char_window_labels["weapon_title"]->setPosition(vline_x, back_pos.y - 180.0f);
        
        _char_window_labels["weapon_value"] = Label::createWithSystemFont(_character->_current_weapon->_name_for_user, "Arial", 24);
        _char_window_labels["weapon_value"]->setAnchorPoint(Vec2(0.0f, 0.5f));
        _char_window_labels["weapon_value"]->setPosition(vline_x + 15.0f, back_pos.y - 180.0f);
        
        for (auto & item : _char_window_labels) layer->addChild(item.second, 2);
        
        _strength_plus = Sprite::create("green_plus.png");
        _strength_plus->setAnchorPoint(Vec2(0.0f, 0.5f));
        _strength_plus->setPosition(vline_x + 40.0f, back_pos.y - 90.0f);
        layer->addChild(_strength_plus, 2);
        
        _accuracy_plus = Sprite::create("green_plus.png");
        _accuracy_plus->setAnchorPoint(Vec2(0.0f, 0.5f));
        _accuracy_plus->setPosition(vline_x + 40.0f, back_pos.y - 113.0f);
        layer->addChild(_accuracy_plus, 2);
        
        hide_char_window();
        
        _initialized = true;
    }
}

void Interface::show_char_window ()
{
    _char_window_is_visible = true;
    _char_window_background1->setVisible(true);
    _char_window_background2->setVisible(true);
    for (auto & item : _char_window_labels) item.second->setVisible(true);
    update();
}

void Interface::hide_char_window ()
{
    _char_window_is_visible = false;
    _char_window_background1->setVisible(false);
    _char_window_background2->setVisible(false);
    for (auto & item : _char_window_labels) item.second->setVisible(false);
    _strength_plus->setVisible(false);
    _accuracy_plus->setVisible(false);
}

bool Interface::is_char_window_visible ()
{
    return _char_window_is_visible;
}

void Interface::on_mouse_click (const Point & point)
{
    if (_char_window_is_visible && _strength_plus->isVisible() && _accuracy_plus->isVisible())
    {
        if (_strength_plus->getBoundingBox().containsPoint(point))
        {
            _character->add_one_strength();
            update();
        }
        else if (_accuracy_plus->getBoundingBox().containsPoint(point))
        {
            _character->add_one_accuracy();
            update();
        }
    }
}

void Interface::update ()
{
    unsigned int current_hp = _character->get_current_hp();
    unsigned int max_hp = _character->get_max_hp();
    unsigned int char_cp = _character->get_free_cp();
    
    unsigned int pustol_bullets_count = _character->get_bullets_count("pistol_bullet");
    _p_bullets_count->setString(NumberToString(pustol_bullets_count));
    
    unsigned int mashine_bullets_count = _character->get_bullets_count("machine_bullet");
    _m_bullets_count->setString(NumberToString(mashine_bullets_count));
    
    _green_line->setScaleX(_hp_line_scale * (static_cast <float> (current_hp) / static_cast <float> (max_hp)));
    _values_hp->setString(NumberToString(current_hp) + " / " + NumberToString(max_hp));
    
    if (char_cp > 0 && !_green_arrow_up->isVisible())
    {
        _green_arrow_up->setVisible(true);
        _green_arrow_up->resume();
    }
    else if (char_cp == 0 && _green_arrow_up->isVisible())
    {
        _green_arrow_up->pause();
        _green_arrow_up->setVisible(false);
    }
    
    if (_char_window_is_visible)
    {
        _char_window_labels["level_value"]->setString(NumberToString(_character->get_level()));
        _char_window_labels["strength_value"]->setString(NumberToString(_character->get_strength()));
        _char_window_labels["accuracy_value"]->setString(NumberToString(_character->get_accuracy()));
        _char_window_labels["cp_value"]->setString(NumberToString(char_cp));
        _char_window_labels["weapon_value"]->setString(_character->_current_weapon->_name_for_user);
        
        _strength_plus->setVisible(char_cp > 0);
        _accuracy_plus->setVisible(char_cp > 0);
    }
}
