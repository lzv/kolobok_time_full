#ifndef TOOLS_H
#define TOOLS_H

#include "cocos2d.h"
USING_NS_CC;
#include "ObjectsParams.h"
#include "BoolBitMap.h"

template <typename T>
std::string NumberToString (T Number)
{
    std::stringstream ss;
    ss << Number;
    return ss.str();
}

template <typename T>
T StringToNumber (const std::string & Text)
{
    std::stringstream ss{Text};
    T result;
    return (ss >> result) ? result : 0;
}

template <>
inline std::string StringToNumber <std::string> (const std::string & Text)
{
    return Text;
}

class IntRect
{
public:
    int x, y;
    unsigned int width, height;
    
    IntRect ();
    IntRect (int x, int y, unsigned int width, unsigned int height);
    IntRect (int x, int y, unsigned int width_and_height);
    IntRect (const IntRect & other);
    IntRect & operator= (const IntRect & other);
    
    // Как в итераторах, endX и endY не включены в прямоугольник.
    int beginX () const;
    int endX () const;
    int beginY () const;
    int endY () const;
    
    bool isZeroArea () const;
    bool isIntersectsRect (const IntRect & rect) const;
    IntRect getIntersectWithRect (const IntRect & rect) const;
};

// По идее нужно будет организовать кэш битовых масок. Но когда в приоритете скорость реализации, 
// кэш можно отложить на то время, когда количество объектов на картах значительно увеличится.
// Идея кэша: при создании карта запрашивает из кэша нужные данные, указывая свой номер.
// В кэше номер карты связывается с именем файла. С одним файлом может быть связано несколько карт.
// В деструкторе карта сообщает кэшу свой номер. В кэше имена файлов отвязываются от этой карты. 
// Если к имени файла не осталось привязано других карт, то ресурсы этого файла удаляются из кэша.
// Учитывая, что новая карта создается, когда старая еще в памяти, освобождаются только ненужные ресурсы.

// image_file_name - относительный путь к файлу картинки в каталоге Resources/ 
// Если картинка с альфа-каналом, то свободный путь обозначается полной прозрачностью. 
// Если без альфа-канала, то препятствия обозначаются полностью черным цветом.
// В битовой карте препятствия обозначаются true, а свободные пиксели - false.
// Объект bit_map должен быть не инициализированным.
void init_bool_bit_map_from_image (BoolBitMap & bit_map, const std::string & image_file_name);

//Rect get_intersection_of_rects (const Rect & rect1, const Rect & rect2);

// Координаты точек должны быть в одной системе отсчета 
// (соответствующие ноды должны принадлежать одному и тому же родителю). 
bool is_intersect_two_bitmaps (
    const Vec2 & left_bottom_point_1, const BoolBitMap & bit_map_1,
    const Vec2 & left_bottom_point_2, const BoolBitMap & bit_map_2);

bool is_intersect_bitmap_and_rect (
    const Vec2 & left_bottom_map_point, const BoolBitMap & bit_map, 
    const Rect & rect);

bool is_intersect_bitmap_and_point (
    const Vec2 & left_bottom_map_point, const BoolBitMap & bit_map,
    const Vec2 & check_point);

// x и y могут не принадлежать bit_map, но часть круга все равно будет нарисована.
void draw_circle_on_bitmap (BoolBitMap & bit_map, int x, int y, unsigned int radius);

// Существование файла не проверяется.
void write_image_from_bitmap (const BoolBitMap & bit_map, const std::string & image_full_file_name);

//SpriteFrame * create_sprite_frame (const std::string & fname);

Animation * create_animation (
    const std::string & fname_template,  /* Шаблон имени файлов относительно каталога Resources/, формат как для printf. */
    unsigned int frames_count,           /* Количество файлов-картинок в анимации. */
    float delay_per_unit,                /* Задержка между двумя кадрами в секундах. */
    bool restore_orig_frame = true,      /* Восстанавливать ли оригинальную картинку после анимации. */
    unsigned int fname_max_lenght = 100, /* Максимально возможная длинна имени файла с подставленным номером. */
    int start_num_from = 0,              /* Подставлять номера в шаблон, начиная с этого номера. */
    int first_frame_anim = 0             /* Сдвинуть (циклически) начало анимации на этот номер файла. */
);

Animation * create_animation (const AnimationParams & params);

float rad_to_degree (float rad);

float degree_to_rad (float degree);

// Возвращается угол в градусах между параметром и вектором (1, 0), положительное направление по часовой стрелке.
float get_angle_of_vector (Vec2 vec);

#endif // TOOLS_H
