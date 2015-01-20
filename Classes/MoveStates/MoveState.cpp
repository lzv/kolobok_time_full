#include "MoveState.h"
#include "../MapActors/MapActor.h"
#include "../Scenes/PlayScreen.h"
#include "../Tools.h"

void MoveState::on_target_dead (MapActor *) {}

bool MoveState::step_along_patch (
    MapActor * ch, 
    std::forward_list <std::pair <int, int>> & patch,
    const float dt,
    bool set_rotate_angle
) {
    auto start_pos = ch->getPosition();
    float allow_distance = ch->_base_speed * dt;
    int point_counter = 0;
    auto current_pos = start_pos;
    
    for (auto iter = patch.begin(); iter != patch.end(); ++iter)
    {
        Point next_pos {static_cast <float> (iter->first), static_cast <float> (iter->second)};
        float distance_to_next = current_pos.distance(next_pos);
        
        if (set_rotate_angle) 
        {
            float angle = get_angle_of_vector(Vec2 {current_pos, next_pos});
            ch->set_rotate_angle(angle);
        }
        
        if (allow_distance > distance_to_next)
        {
            allow_distance -= distance_to_next;
            current_pos = next_pos;
            ++point_counter;
        }
        else {
            Vec2 vec(current_pos, next_pos);
            vec.normalize();
            current_pos += vec * allow_distance;
            break;
        }
    }
    
    ch->setPosition(current_pos); // Изменение только позиции, без дополнительных действий.
    if (ch->_main_screen->isIntersectMapActorWithMoveObstructions(ch))
    {
        ch->setPosition(start_pos); // Возврат предыдущей позиции 
        return false;
    }
    else {
        while (point_counter > 0)
        {
            patch.pop_front();
            --point_counter;
        }
        return true;
    }
}
