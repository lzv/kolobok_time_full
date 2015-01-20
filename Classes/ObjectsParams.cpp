#include "ObjectsParams.h"
#include <stdexcept>
#include "Tools.h"

//--------------- Разное вспомогательное ------------------------

AnimationParams::AnimationParams ()
    : fname_template(""), frames_count(0), delay_per_unit(0.0f), restore_orig_frame(true),
    fname_max_lenght(100), start_num_from(0), first_frame_anim(0)
{}

AnimationParams::AnimationParams (
    const std::string & fname_template,
    unsigned int frames_count,
    float delay_per_unit,
    bool restore_orig_frame,
    unsigned int fname_max_lenght,
    int start_num_from,
    int first_frame_anim
)
    : fname_template(fname_template), frames_count(frames_count), delay_per_unit(delay_per_unit), 
    restore_orig_frame(restore_orig_frame), fname_max_lenght(fname_max_lenght), 
    start_num_from(start_num_from), first_frame_anim(first_frame_anim)
{}

bool AnimationParams::is_valid () const
{
    return fname_template.length() > 0 && frames_count > 0;
}

//--------------- Bullet ------------------------

static const std::vector <BulletParams> all_bullet_params {
    {"machine_bullet", "machine_bullets", "Патроны для автомата", "objects/machine_bullet.png"},
    {"pistol_bullet", "pistol_bullets", "Патроны для пистолета", "objects/pistol_bullet.png"}
};

BulletParams get_bullet_params (const std::string & code)
{
    for (auto & par : all_bullet_params)
        if (par.code == code) return par;
    throw std::runtime_error("Not found bullet code " + code);
}

//--------------- Weapon ------------------------

static const std::vector <WeaponParams> all_weapon_params {
    {"small_sword", WeaponType::MELEE, "", "Кинжал", "weapon/small_sword.png", 0.25f, 3, 37.0f, 46.0f, 10.0f, 0.0f, 0.0f},
    {"long_sword", WeaponType::MELEE, "", "Меч", "weapon/long_sword.png", 0.7f, 10, 79.0f, 46.0f, 10.0f, 0.0f, 0.0f},
    {"axe", WeaponType::MELEE, "", "Топор", "weapon/axe.png", 1.1f, 15, 49.0f, 44.0f, 8.0f, 0.0f, 0.0f},
    {"postol", WeaponType::FIRE, "pistol_bullet", "Пистолет", "weapon/postol.png", 0.15f, 3, 300.0f, 30.0f, 22.0f, 25.0f, 15.0f},
    {"mashine",  WeaponType::FIRE, "machine_bullet", "Автомат", "weapon/mashine.png", 0.07f, 2, 550.0f, 15.0f, 22.0f, 55.0f, 9.0f}
};

WeaponParams get_weapon_params (const std::string & code)
{
    for (auto & par : all_weapon_params)
        if (par.code == code) return par;
    throw std::runtime_error("Not found weapon code " + code);
}

//--------------- Transitions ------------------------

static const std::vector <VisibleTransitionParams> all_visible_transitions_params {
    {"portal", "building/portal/portal_0.png", 
        {"building/portal/portal_%d.png", 2, 0.5f, false, 40, 0, 0}}
};

VisibleTransitionParams get_visible_transition_params (const std::string & code)
{
    for (auto & par : all_visible_transitions_params)
        if (par.code == code) return par;
    throw std::runtime_error("Not found visible transition params, code " + code);
}

//--------------- Maps ------------------------

static const std::vector <MapParams> all_maps_params {
    {1, "maps/01_start.png", "maps/01_move_map.png", "maps/01_move_map_fp.png", "maps/01_attack_map.png", {
        {"portal", "Портал далее", 410.0f, 215.0f, 0.5f, 0.5f, 0.0f, 0.0f, 3, 130.0f, 130.0f},
        {"", "Вход в дом", 964.0f, 1008.0f, 0.0f, 0.5f, 29.0f, 104.0f, 2, 163.0f, 125.0f}
    }},
    {2, "maps/02_house.png", "maps/02_move_map.png", "maps/02_move_map_fp.png", "maps/02_attack_map.png", {
        {"", "Выход из дома", 161.0f, 160.0f, 0.5f, 0.0f, 79.0f, 40.0f, 1, 926.0f, 1007.0f}
    }},
    {3, "maps/03_end.png", "maps/03_move_map.png", "maps/03_move_map_fp.png", "maps/03_attack_map.png", {
        {"portal", "Портал назад", 45.0f, 30.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1, 527.0f, 213.0f}
    }}
};

MapParams get_map_params (unsigned int number)
{
    for (auto & par : all_maps_params)
        if (par.number == number) return par;
    throw std::runtime_error("Not found map params number " + NumberToString(number));
}

//--------------- Health potion ------------------------

static const std::vector <HealthPotionMapParams> all_health_potions_params {
    {"health_small", "Малое зелье лечения", "objects/health_potion_small.png", 10},
    {"health_big", "Большое зелье лечения", "objects/health_potion_big.png", 25}
};

HealthPotionMapParams get_health_potion_map_params (const std::string & code)
{
    for (auto & par : all_health_potions_params)
        if (par.code == code) return par;
    throw std::runtime_error("Not found health potion params, code " + code);
}

//--------------- Enemies ------------------------

static const std::vector <EnemyTypesParams> all_enemy_types {
    {"green", "Зеленый", "enemies/green.png"},
    {"blue", "Синий", "enemies/blue.png"},
    {"braun", "Коричневый", "enemies/braun.png"},
    {"pink", "Розовый", "enemies/pink.png"},
    {"red", "Красный", "enemies/red.png"}
};

EnemyTypesParams get_enemy_map_params (const std::string & code)
{
    for (auto & par : all_enemy_types)
        if (par.code == code) return par;
    throw std::runtime_error("Not found enemy params, code " + code);
}
