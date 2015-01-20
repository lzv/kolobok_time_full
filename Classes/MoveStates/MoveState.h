#ifndef MOVESTATE_H
#define MOVESTATE_H

#include "cocos2d.h"
USING_NS_CC;
#include <forward_list>

class MapActor;

class MoveState
{
public:
    MoveState () = default;
    virtual ~MoveState () = default;
    
    // Проверка допустимых входящих сигналов для текущего состояния и установка нового состояния персонажа.
    virtual void check_input (MapActor *) = 0;
    // Установка новых параметров персонажа, таких как позиция, угол поворота (смотреть вперед по движению).
    virtual void update_character (float dt, MapActor *) = 0;
    // Если идет преследование, и цель оказалась мертва, нужно сбросить состояния преследования.
    virtual void on_target_dead (MapActor *);
    
protected:
    // Перемещение объекта MapActor по последовательности точек, найденных алгоритмом поиска пути.
    // Если возвращается false, то шаг не удался и персонаж остался на прежнем месте.
    // Из списка patch будут удалены пройденные точки.
    // actor не должен быть nullptr. patch не должен быть пустым. Проверок не производится.
    bool step_along_patch (
        MapActor * actor, 
        std::forward_list <std::pair <int, int>> & patch,
        const float dt,
        bool set_rotate_angle = false
    );
};

#endif // MOVESTATE_H
