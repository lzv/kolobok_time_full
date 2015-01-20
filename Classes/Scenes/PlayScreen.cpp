#include "PlayScreen.h"
#include "PauseScreen.h"
#include "DeadScreen.h"
#include "../DBManager.h"
#include "../InputManager.h"
#include "../MapActors/Character.h"
#include "../MapActors/Enemy.h"
#include "../ObjectsParams.h"
#include "../Tools.h"
#include "../MapObject.h"
#include "../CharWeapon.h"

PlayScreen::~PlayScreen ()
{
    _black_pixel->release();
}

void PlayScreen::create_layers (const Size & map_content_size, const Size & window_content_size)
{
    std::string names [] = {"floor", "objects", "char_and_enemies", "interface"};
    int z = 0;
    for (auto name : names)
    {
        _layers[name] = Layer::create();
        _layers[name]->setAnchorPoint(Vec2(0.0f, 0.0f));
        _layers[name]->setPosition(0.0f, 0.0f);
        _layers[name]->ignoreAnchorPointForPosition(false);
        addChild(_layers[name], z++);
    }
    
    // По умолчанию у слоев ContentSize равен размеру окна программы, исправляем.
    _layers["floor"]->setContentSize(map_content_size);
    _layers["objects"]->setContentSize(map_content_size);
    _layers["char_and_enemies"]->setContentSize(map_content_size);
    
    // Отцентрируем слои в окне программы. Если их размер меньше окна, то их положение больше не изменится.
    Vec2 position(
        (window_content_size.width - map_content_size.width) / 2.0f, 
        (window_content_size.height - map_content_size.height) / 2.0f
    );
    _layers["floor"]->setPosition(position);
    _layers["objects"]->setPosition(position);
    _layers["char_and_enemies"]->setPosition(position);
}

bool PlayScreen::init ()
{
    if (!GameScreen::init()) return false;
    
    _current_map_number = DBManager::I().get_character_value <unsigned int> ("map_number");
    auto map_params = get_map_params(_current_map_number);
    
    auto map_sp = Sprite::create(map_params.img_file_name);
    map_sp->setAnchorPoint(Vec2(0.0f, 0.0f));
    
    map_content_size = map_sp->getContentSize();
    window_size = Director::getInstance()->getWinSize();
    create_layers(map_content_size, window_size);
    
    _layers["floor"]->addChild(map_sp);
    
    init_bool_bit_map_from_image(_move_bit_map, map_params.move_map_filename);
    init_bool_bit_map_from_image(_attack_bit_map, map_params.attak_map_filename);
    _bit_map_width = _move_bit_map._width;
    _bit_map_height = _move_bit_map._height;
    
    CCASSERT(_move_bit_map._width == _attack_bit_map._width, "PlayScreen - bit map width does not match");
    CCASSERT(_move_bit_map._width > 0, "PlayScreen - bit map width is 0");
    CCASSERT(_move_bit_map._height == _attack_bit_map._height, "PlayScreen - bit map height does not match");
    CCASSERT(_move_bit_map._height > 0, "PlayScreen - bit map height is 0");
    
    create_move_bit_map_find_patch(map_params.move_map_filename_find_patch);
    
    _character = Character::create(this);
    _character->setPosition(
        DBManager::I().get_character_value <float> ("pos_x"), 
        DBManager::I().get_character_value <float> ("pos_y")
    );
    _layers["char_and_enemies"]->addChild(_character, 10);
    
    auto map_enemies = DBManager::I().get_enemies_on_map(_current_map_number);
    for (EnemyDBParams & en_db_par : map_enemies)
    {
        auto obj = Enemy::create(en_db_par, this);
        obj->setPosition(en_db_par.x, en_db_par.y);
        _layers["char_and_enemies"]->addChild(obj, 10);
        _enemies.push_back(obj);
    }
    
    _black_pixel = SpriteFrame::create("black_pixel.png", Rect{0.0f, 0.0f, 1.0f, 1.0f});
    _black_pixel->retain();
    
    auto weapons_on_map = DBManager::I().get_weapons_on_map(_current_map_number);
    for (WeaponsDBParams & wp_db_par : weapons_on_map)
    {
        auto obj = MapWeapon::create(get_weapon_params(wp_db_par.code));
        obj->setPosition(wp_db_par.x, wp_db_par.y);
        obj->set_db_id(wp_db_par.id);
        _layers["objects"]->addChild(obj);
        _map_objects.push_back(obj);
    }
    
    auto bullets_on_map = DBManager::I().get_bullets_on_map(_current_map_number);
    for (BulletsDBParams & bl_db_par : bullets_on_map)
    {
        auto obj = MapBullets::create(get_bullet_params(bl_db_par.code), bl_db_par.count);
        obj->setPosition(bl_db_par.x, bl_db_par.y);
        obj->set_db_id(bl_db_par.id);
        _layers["objects"]->addChild(obj);
        _map_objects.push_back(obj);
    }
    
    auto simple_objects_on_map = DBManager::I().get_simple_objects_on_map(_current_map_number);
    for (SimpleObjectDBParams & so_db_par : simple_objects_on_map)
    {
        MapObject * obj = nullptr;
        if (so_db_par.type == SimpleObjectType::HEALTH_POTION)
            obj = MapHealthPotion::create(get_health_potion_map_params(so_db_par.code));
        if (obj)
        {
            obj->setPosition(so_db_par.x, so_db_par.y);
            obj->set_db_id(so_db_par.id);
            _layers["objects"]->addChild(obj);
            _map_objects.push_back(obj);
        }
    }
    
    for (TransitionOnMapParams & par : map_params.transitions)
    {
        auto obj = create_map_transition(par);
        obj->setPosition(par.pos_x, par.pos_y);
        _layers["objects"]->addChild(obj);
        _map_objects.push_back(obj);
    }
    
    _interface.init(_layers["interface"], _character);
    
    set_layers_position();
    scheduleUpdate();
    
    return true;
}

PointType PlayScreen::getPointType (const Point & click_point)
{
    _last_found_map_object = nullptr;
    _last_found_enemy = nullptr;
    
    Point map_point = click_point - _layers["floor"]->getPosition();
    int x = static_cast <int> (map_point.x + 0.5f);
    int y = static_cast <int> (map_point.y + 0.5f);
    
    if (
        x >= 0 && x < static_cast <int> (_bit_map_width) 
        && y >= 0 && y < static_cast <int> (_bit_map_height)
        && !_move_bit_map.at(x, y)
    ) {
        for (Enemy * obj : _enemies)
            if (obj->is_intersect_with_point(map_point))
            {
                _last_found_enemy = obj;
                return PointType::ENEMY;
            }
        
        for (MapObject * obj : _map_objects)
            if (obj->is_intersect_with_point(map_point))
            {
                _last_found_map_object = obj;
                return PointType::OBJECT;
            }
        
        if (_character->is_intersect_with_point(map_point))
            return PointType::CHARACTER;
        
        return PointType::ACCESSIBLE;
    }
    return PointType::NOT_ACCESSIBLE;
}

MapObject * PlayScreen::getLastFoundMapObject ()
{
    return _last_found_map_object;
}

Enemy * PlayScreen::getLastFoundEnemy ()
{
    return _last_found_enemy;
}

Point PlayScreen::getLayersPos ()
{
    return _layers["floor"]->getPosition();
}

void PlayScreen::set_layers_position ()
{
    if (map_content_size.width > window_size.width)
    {
        float min_layers_x = window_size.width - map_content_size.width;
        float layers_pos_x = window_size.width / 2.0f - _character->getPositionX();
        if (layers_pos_x > 0.0f) layers_pos_x = 0.0f;
        else if (layers_pos_x < min_layers_x) layers_pos_x = min_layers_x;
        _layers["floor"]->setPositionX(layers_pos_x);
        _layers["objects"]->setPositionX(layers_pos_x);
        _layers["char_and_enemies"]->setPositionX(layers_pos_x);
    }
    if (map_content_size.height > window_size.height)
    {
        float min_layers_y = window_size.height - map_content_size.height;
        float layer_pos_y = window_size.height / 2.0f - _character->getPositionY();
        if (layer_pos_y > 0.0f) layer_pos_y = 0.0f;
        else if (layer_pos_y < min_layers_y) layer_pos_y = min_layers_y;
        _layers["floor"]->setPositionY(layer_pos_y);
        _layers["objects"]->setPositionY(layer_pos_y);
        _layers["char_and_enemies"]->setPositionY(layer_pos_y);
    }
}

void PlayScreen::onCharacterDead ()
{
    set_pause(true);
    pause();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, DeadScreen::create(), Color3B::BLACK));
}

void PlayScreen::onPauseGame ()
{
    set_pause(true);
    Director::getInstance()->pushScene(PauseScreen::create());
}

void PlayScreen::update (float delta)
{
    if (!_on_pause)
    {
        _character->check_move_and_attack(delta);
        for (Enemy * en : _enemies) en->check_move_and_attack(delta);
        for (MapObject * obj : _map_objects) obj->check_touch(_character);
        set_layers_position();
    }
}

void PlayScreen::set_pause (bool is_pause)
{
    _on_pause = is_pause;
    if (is_pause)
    {
        for (Enemy * en : _enemies) en->_current_weapon->pause();
        _character->_current_weapon->pause();
    }
    else {
        for (Enemy * en : _enemies) en->_current_weapon->resume();
        _character->_current_weapon->resume();
    }
}

void PlayScreen::onEnterTransitionDidFinish ()
{
    GameScreen::onEnterTransitionDidFinish();
    
    InputManager::I().key_press_callback = [this](EventKeyboard::KeyCode code){
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE)
        {
            onPauseGame();
        }
        else if (code == EventKeyboard::KeyCode::KEY_C)
        {
            if (_interface.is_char_window_visible())
            {
                set_pause(false);
                _interface.hide_char_window();
            }
            else {
                set_pause(true);
                _interface.show_char_window();
            }
        }
    };
    
    InputManager::I().mouse_down_callback = [this](int key_number){
        if (key_number == 0) _interface.on_mouse_click(InputManager::I().get_mouse_pos());
    };
    
    if (_on_pause) set_pause(false);
}

void PlayScreen::deleteMapObject (MapObject * obj)
{
    auto iter = std::find(_map_objects.begin(), _map_objects.end(), obj);
    if (iter != _map_objects.end())
    {
        obj->delete_self_from_db();
        _layers["objects"]->removeChild(obj);
        _map_objects.erase(iter);
    }
}

void PlayScreen::addMapObject (MapObject * obj)
{
    if (obj)
    {
        obj->add_self_into_db(_current_map_number);
        _layers["objects"]->addChild(obj);
        _map_objects.push_back(obj);
    }
}

void PlayScreen::updateInterface ()
{
    _interface.update();
}

void PlayScreen::on_attack (MapActor * actor)
{
    if (actor->_current_weapon->_type == WeaponType::MELEE)
    {
        float angle = degree_to_rad(actor->get_rotate_angle());
        Vec2 direction {static_cast <float> (cos(angle)), static_cast <float> (-sin(angle))};
        Point start_point = actor->getPosition() + direction * 25.0f;
        float distance = actor->_current_weapon->_radius;
        Point current_point = start_point;
        unsigned int damage = actor->_current_weapon->_base_damage * (1.0f + actor->get_strength() / 10.0f);
        
        while (distance > 0.0f)
        {
            int int_x = static_cast <int> (current_point.x);
            int int_y = static_cast <int> (current_point.y);
            
            if (_attack_bit_map.is_inside(int_x, int_y))
            {
                if (_attack_bit_map.at(int_x, int_y)) break;
                
                if (actor == _character)
                {
                    for (auto iter = _enemies.begin(); iter != _enemies.end(); ++iter)
                    {
                        Enemy * en = *iter;
                        if (en->is_intersect_with_point(current_point))
                        {
                            if (en->get_current_hp() > damage)
                            {
                                en->add_damage(damage);
                            }
                            else {
                                _character->add_xp(en->get_xp_price());
                                actor->on_target_dead();
                                en->remove_self_from_db();
                                _layers["char_and_enemies"]->removeChild(en, true);
                                _enemies.erase(iter);
                            }
                            distance = -1.0f;
                            break;
                        }
                    }
                }
                else {
                    if (_character->is_intersect_with_point(current_point))
                    {
                        if (_character->get_current_hp() > damage)
                        {
                            _character->add_damage(damage);
                        }
                        else {
                            for (Enemy * en : _enemies) en->_current_weapon->pause();
                            _character->_current_weapon->pause();
                            pause();
                            onCharacterDead();
                        }
                        break;
                    }
                }
            }
            else {
                break;
            }
            current_point += direction * 2.0f;
            distance -= 2.0f;
        }
    }
    else if (actor->_current_weapon->_type == WeaponType::FIRE)
    {
        float char_angle = degree_to_rad(actor->get_rotate_angle());
        Vec2 char_direction {static_cast <float> (cos(char_angle)), static_cast <float> (-sin(char_angle))};
        Point start_point = actor->getPosition() + char_direction * 27.0f;
        
        unsigned int accuracy = actor->get_accuracy();
        float angle_deflection = 0.0f;
        if (accuracy < 30)
        {
            float temp = 30.0f - static_cast <float> (accuracy);
            angle_deflection = (static_cast <float> (rand()) / RAND_MAX) * temp - temp / 2.0f;
        }
        float angle = degree_to_rad(actor->get_rotate_angle() + angle_deflection);
        Vec2 direction {static_cast <float> (cos(angle)), static_cast <float> (-sin(angle))};
        
        float distance = actor->_current_weapon->_radius - 2.0f;
        
        unsigned int damage = actor->_current_weapon->_base_damage;
        if (accuracy >= 30)
        {
            float new_damage = static_cast <float> (damage) * (1.0f + static_cast <float> (accuracy - 29) / 10.0f);
            damage = static_cast <unsigned int> (new_damage + 0.5f);
        }
        
        Point current_point = start_point;
        Point line_end_point = start_point;
        bool show_after_while = true;
        
        while (distance > 0.0f)
        {
            int int_x = static_cast <int> (current_point.x);
            int int_y = static_cast <int> (current_point.y);
            
            if (_attack_bit_map.is_inside(int_x, int_y))
            {
                if (_attack_bit_map.at(int_x, int_y)) break;
                
                if (actor == _character)
                {
                    for (auto iter = _enemies.begin(); iter != _enemies.end(); ++iter)
                    {
                        Enemy * en = *iter;
                        if (en->is_intersect_with_point(current_point))
                        {
                            if (en->get_current_hp() > damage)
                            {
                                en->add_damage(damage);
                            }
                            else {
                                _character->add_xp(en->get_xp_price());
                                actor->on_target_dead();
                                en->remove_self_from_db();
                                _layers["char_and_enemies"]->removeChild(en, true);
                                _enemies.erase(iter);
                            }
                            show_bullet_line(start_point, line_end_point);
                            show_after_while = false;
                            
                            distance = -1.0f;
                            break;
                        }
                    }
                }
                else {
                    if (_character->is_intersect_with_point(current_point))
                    {
                        if (_character->get_current_hp() > damage)
                        {
                            _character->add_damage(damage);
                        }
                        else {
                            show_bullet_line(start_point, line_end_point);
                            show_after_while = false;
                            
                            for (Enemy * en : _enemies) en->_current_weapon->pause();
                            _character->_current_weapon->pause();
                            pause();
                            onCharacterDead();
                        }
                        break;
                    }
                }
            }
            else {
                break;
            }
            
            line_end_point = current_point;
            current_point += direction * 2.0f;
            distance -= 2.0f;
        }
        
        if (show_after_while) show_bullet_line(start_point, line_end_point);
    }
}

bool PlayScreen::isIntersectMapActorWithMoveObstructions (MapActor * actor)
{
    if (actor != nullptr)
    {
        int x = static_cast <int> (actor->getPositionX() + 0.5f);
        int y = static_cast <int> (actor->getPositionY() + 0.5f);
        if (_move_bit_map_find_patch.at(x, y)) return true;
        
        for (Enemy * en : _enemies)
            if (
                en != actor 
                && is_intersect_two_bitmaps(en->get_left_bottom_bit_map_point(), en->_bit_map, 
                                            actor->get_left_bottom_bit_map_point(), actor->_bit_map)
            )
                return true;
        
        if (
            actor != _character 
            && is_intersect_two_bitmaps(_character->get_left_bottom_bit_map_point(), _character->_bit_map, 
                                        actor->get_left_bottom_bit_map_point(), actor->_bit_map)
        )
            return true;
    }
    
    return false;
}

// _move_bit_map - свободных пикселей 3636767, занятых 363233. 

/*
Распространенные алгоритмы поиска путей предполагают, что персонаж занимает одну ячейку поля. Но в данной игре 
это не так - персонаж является кругом диаметом 50 пикселей, и в то же время может передвигаться попиксельно. 
Поэтому было придумано решение - сделать вторую битовую карту перемещения, в которой нарастить препятствия 
на 25 пикселей в стороны. Тогда по этой карте может искать путь персонаж, занимающий один пиксель - центр 
настоящего персонажа, при этом он не будет задевать настоящие препятствия. 

Есть два пути решения: для каждого занятого пикселя на старой битовой карте рисовать занятый круг на новой 
битовой карте; и для каждого еще свободного пикселя на новой битовой карте искать занятый пиксель на старой 
битовой карте в пределах окружности. В пользу первого варианта то, что занятых пикселей примерно в 10 раз меньше 
свободных. Но в пользу второго то, что поиск остановится на первом найденном пикселе, тогда как в первом варианте 
закрашивать окружность нужно будет каждый раз полностью. 
Чем тоньше линии препятствий, тем первый вариант предпочтительней. И наоборот, чем толще линии и чем больше области 
препятствий, тем второй вариант лучше. 

Однако, вышесказанное имеет смысл, если поиск пикселей производится по порядку - по очереди в первой линии пикселей, 
затем вторая линия, и т.д. Если же искать пиксели в массиве случайным образом, то второй вариант может дать лучший 
результат даже на тонких линиях препятствий. Конечно, генерация случайных чисел занимает время. Но так как размер 
массива постоянный - 50*50, то можно один раз сформировать последовательность поиска, и использовать ее во всех случаях. 

Здесь я выбрал первый вариант, так как время работы не важно - результат будет сохранен в изображение, и при следующих 
запусках просто прочитан с диска.
*/
void PlayScreen::create_move_bit_map_find_patch (const std::string & file_name)
{
    if (FileUtils::getInstance()->isFileExist(file_name))
    {
        init_bool_bit_map_from_image(_move_bit_map_find_patch, file_name);
    }
    else {
        _move_bit_map_find_patch.init(_bit_map_width, _bit_map_height);
        std::vector <std::thread *> threads;
        threads.reserve(300);
        
        for (unsigned int x = 0; x < _bit_map_width; ++x)
            for (unsigned int y = 0; y < _bit_map_height; ++y)
                if (_move_bit_map.at(x, y))
                {
                    std::thread * fun = new std::thread(draw_circle_on_bitmap, std::ref(_move_bit_map_find_patch), x, y, 25);
                    threads.push_back(fun);
                    
                    if (threads.size() >= 300)
                    {
                        for (std::thread * fun : threads) 
                        {
                            (*fun).join();
                            delete fun;
                        }
                        threads.clear();
                    }
                }
        for (std::thread * fun : threads) 
        {
            (*fun).join();
            delete fun;
        }
        
        std::string full_file_name = FileUtils::getInstance()->getSearchPaths()[0] + file_name;
        write_image_from_bitmap(_move_bit_map_find_patch, full_file_name);
    }
}

// Координаты точки клика могут прийти в точку, отмеченную свободной на _move_bit_map, но отмеченную 
// препятствием на _b_move_bit_map_find_patch. В таком случае, в радиусе 25 пикселов ищем ближайшую 
// свободную точку на _b_move_bit_map_find_patch, и ищем путь в нее.
JPSFindResults PlayScreen::find_patch_from_MapActor (MapActor * actor, const Point & end_map_point)
{
    int begin_x = static_cast <int> (actor->getPositionX() + 0.5f);
    int begin_y = static_cast <int> (actor->getPositionY() + 0.5f);
    int end_x = static_cast <int> (end_map_point.x + 0.5f);
    int end_y = static_cast <int> (end_map_point.y + 0.5f);
    
    if (_move_bit_map_find_patch.is_inside(end_x, end_y))
    {
        if (_move_bit_map_find_patch.at(end_x, end_y))
        {
            bool point_found = false;
            
            for (int radius = 1; radius < 26; ++ radius)
            {
                int from_x = end_x - radius;
                int to_x = end_x + radius;
                int from_y = end_y - radius;
                int to_y = end_y + radius;
                
                for (int i = from_x; i <= to_x; ++i)
                    if (
                        _move_bit_map_find_patch.is_inside(i, from_y) 
                        && !_move_bit_map_find_patch.at(i, from_y)
                    ) {
                        end_x = i;
                        end_y = from_y;
                        point_found = true;
                        break;
                    }
                
                if (!point_found)
                    for (int i = from_x; i <= to_x; ++i)
                        if (
                            _move_bit_map_find_patch.is_inside(i, to_y) 
                            && !_move_bit_map_find_patch.at(i, to_y)
                        ) {
                            end_x = i;
                            end_y = to_y;
                            point_found = true;
                            break;
                        }
                
                if (!point_found)
                    for (int i = from_y + 1; i < to_y; ++i)
                        if (
                            _move_bit_map_find_patch.is_inside(from_x, i)
                            && !_move_bit_map_find_patch.at(from_x, i)
                        ) {
                            end_x = from_x;
                            end_y = i;
                            point_found = true;
                            break;
                        }
                
                if (!point_found)
                    for (int i = from_y + 1; i < to_y; ++i)
                        if (
                            _move_bit_map_find_patch.is_inside(to_x, i)
                            && !_move_bit_map_find_patch.at(to_x, i)
                        ) {
                            end_x = to_x;
                            end_y = i;
                            point_found = true;
                            break;
                        }
                
                if (point_found) break;
            }
        }
        
        return JPSFindPatch(_move_bit_map_find_patch, begin_x, begin_y, end_x, end_y);
    }
    else {
        return JPSFindResults {JPSFindStatus::OUT_OF_MAP, {}};
    }
}

void PlayScreen::show_bullet_line (Point start, Point end)
{
    Sprite * sp = Sprite::createWithSpriteFrame(_black_pixel->clone());
    sp->setAnchorPoint(Vec2(0.0f, 0.5f));
    sp->setScaleX(start.distance(end));
    sp->setRotation(get_angle_of_vector(Vec2{start, end}));
    sp->setPosition(start);
    _layers["char_and_enemies"]->addChild(sp, 9);
    sp->runAction(Sequence::create(
        DelayTime::create(0.03f),
        RemoveSelf::create(true),
        nullptr
    ));
}
