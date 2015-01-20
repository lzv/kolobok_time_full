#ifndef BOOLBITMAP_H
#define BOOLBITMAP_H

#include "Macros.h"

// Члены класса public, что бы к ним можно было обращаться напрямую, для большей скорости работы программы. 
// Значение _width * _height должно поместиться в unsigned int.
class BoolBitMap
{
public:
    bool * _p;
    unsigned int _width;
    unsigned int _height;
    
    BoolBitMap ();
    ~BoolBitMap ();
    
    // Возвращает, инициализирован ли объект. 
    bool is_init () const;
    // Выделение памяти. Если нулевой размер или память уже выделена, бросается исключение.
    void init (unsigned int width, unsigned int height, bool init_value = false);
    // Установка всех значений с другого объекта BoolBitMap.
    void copy_from (const BoolBitMap & obj);
    // Координаты не проверяются.
    bool at (unsigned int x, unsigned int y) const;
    // Координаты не проверяются.
    void set (unsigned int x, unsigned int y, bool value);
    // Возвращает, находятся ли координаты внутри карты.
    bool is_inside (int x, int y);
    
    DELETE_COPY_AND_MOVE(BoolBitMap);
};

#endif // BOOLBITMAP_H
