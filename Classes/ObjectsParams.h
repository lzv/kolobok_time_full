#ifndef OBJECTSTYPES_H
#define OBJECTSTYPES_H

#include <string>
#include <vector>

//--------------- Разное вспомогательное ------------------------

// Содержит все параметры для создания объекта анимации.
class AnimationParams 
{
public:
    std::string fname_template;    // Шаблон имени файлов относительно каталога Resources/, формат как для printf.
    unsigned int frames_count;     // Количество файлов-картинок в анимации.
    float delay_per_unit;          // Задержка между двумя кадрами в секундах.
    bool restore_orig_frame;       // Восстанавливать ли оригинальную картинку после анимации.
    unsigned int fname_max_lenght; // Максимально возможная длинна имени файла с подставленным номером.
    int start_num_from;            // Подставлять номера в шаблон, начиная с этого номера.
    int first_frame_anim;          // Сдвинуть (циклически) начало анимации на этот номер файла.
    
    AnimationParams ();
    AnimationParams (
        const std::string & fname_template,
        unsigned int frames_count,
        float delay_per_unit,
        bool restore_orig_frame = true,
        unsigned int fname_max_lenght = 100,
        int start_num_from = 0,
        int first_frame_anim = 0
    );
    bool is_valid () const;
};

//--------------- Bullet ------------------------

struct BulletParams
{
    std::string code;              // Кодовое имя объекта, идентификатор.
    std::string character_db_code; // Код параметра персонажа в таблице БД, в котором хранится количество патронов.
    std::string name_for_user;     // Нормальное имя объекта (для игрока).
    std::string img_file_name;     // Имя файла картинки оружия в каталоге Resources/
};

BulletParams get_bullet_params (const std::string & code);

//--------------- Weapon ------------------------

enum class WeaponType {MELEE, FIRE};

struct WeaponParams 
{
    std::string code;          // Кодовое имя объекта, идентификатор.
    WeaponType type;           // Тип оружия - ближнего боя или огнестрельное.
    std::string bullet_code;   // Код используемых патронов. Для оружия ближнего боя - пустая строка.
    std::string name_for_user; // Нормальное имя объекта (для игрока).
    std::string img_file_name; // Имя файла картинки оружия в каталоге Resources/
    float one_attack_time;     // Время в секундах на одну атаку.
    unsigned int base_damage;  // Базовый урон.
    float radius;              // Радиус действия оружия.
    float char_pos_x;          // Позиция на персонаже x.
    float char_pos_y;          // Позиция на персонаже y.
    float flash_pos_x;         // Позиция x спрайта вспышки на огнестрельном оружии.
    float flash_pos_y;         // Позиция y спрайта вспышки на огнестрельном оружии.
};

WeaponParams get_weapon_params (const std::string & code);

//--------------- Transitions ------------------------

// Параметры типа перехода, например портал, дверь, и т.п. Анимация будет зациклена.
struct VisibleTransitionParams
{
    std::string code;            // Код типа перехода.
    std::string img_file_name;   // Имя файла картинки в каталоге Resources/ 
    AnimationParams anim_params; // Параметры анимации. Если is_valid возвращает false, то анимация не используется.
};

VisibleTransitionParams get_visible_transition_params (const std::string & code);

//--------------- Maps ------------------------

// Параметры перехода на карте
struct TransitionOnMapParams
{
    std::string type_code;            // Код типа перехода. Если пустая строка, то переход невидимый.
    std::string name_for_user;        // Нормальное имя объекта (для игрока).
    float pos_x, pos_y;               // Позиция объекта перехода на карте.
    float ap_x, ap_y;                 // AnchorPoint объекта перехода, значения от 0 до 1.
    float width, height;              // Если переход невидимый (нет спрайта), то используются эти параметры.
    unsigned int target_map_number;   // Номер карты назначения.
    float target_pos_x, target_pos_y; // Координаты прибытия на карте назначения.
};

// Параметры карты 
struct MapParams 
{
    unsigned int number;                             // Номер карты.
    std::string img_file_name;                       // Имя файла изображения поверхности.
    std::string move_map_filename;                   // Имя файла черно-белой карты препятствий для движения.
    std::string move_map_filename_find_patch;        // Карта для алгоритма поиска пути.
    std::string attak_map_filename;                  // Имя файла черно-белой карты препятствий для атаки.
    std::vector <TransitionOnMapParams> transitions; // Переходы с текущей карты на другие.
};

MapParams get_map_params (unsigned int number);

//--------------- Health potion ------------------------

// Параметры лечебных зелий 
struct HealthPotionMapParams
{
    std::string code;            // Код типа зелья.
    std::string name_for_user;   // Нормальное имя объекта (для игрока).
    std::string img_file_name;   // Имя файла изображения.
    unsigned int hp_count;       // Сколько hp лечит.
};

HealthPotionMapParams get_health_potion_map_params (const std::string & code);

//--------------- Enemies ------------------------

struct EnemyTypesParams
{
    std::string code;          // Код типа врага.
    std::string name_for_user; // Нормальное имя объекта (для игрока).
    std::string img_file_name; // Имя файла изображения.
};

EnemyTypesParams get_enemy_map_params (const std::string & code);

#endif // OBJECTSTYPES_H
