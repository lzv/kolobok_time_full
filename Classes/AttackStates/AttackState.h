#ifndef ATTACKSTATE_H
#define ATTACKSTATE_H

#include "cocos2d.h"
USING_NS_CC;

/*
Состояния атаки проверяются после состояний движения.
Действия над персонажем: поворот в нужную сторону при необходимости; 
вызов методов начала и остановки атаки на объекте оружия MapActor-а. 
*/

class MapActor;

class AttackState
{
public:
    AttackState () = default;
    virtual ~AttackState () = default;
    
    // Проверка допустимых входящих сигналов для текущего состояния и установка нового состояния персонажа.
    virtual void check_input (MapActor *) = 0;
    // Необходимые периодические действия.
    virtual void actions (float dt, MapActor *) = 0;
    // Если идет преследование, и цель оказалась мертва, нужно сбросить состояния преследования.
    virtual void on_target_dead (MapActor *);
};

#endif // ATTACKSTATE_H
