#include "Tools.h"
#include <stdexcept>

IntRect::IntRect ()
    : x(0), y(0), width(0), height(0)
{}

IntRect::IntRect (int x, int y, unsigned int width, unsigned int height)
    : x(x), y(y), width(width), height(height)
{}

IntRect::IntRect (int x, int y, unsigned int width_and_height)
    : x(x), y(y), width(width_and_height), height(width_and_height)
{}

IntRect::IntRect (const IntRect & other)
    : x(other.x), y(other.y), width(other.width), height(other.height)
{}

IntRect & IntRect::operator= (const IntRect & other)
{
    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    return *this;
}

int IntRect::beginX () const {return x;}

int IntRect::endX () const {return x + width;}

int IntRect::beginY () const {return y;}

int IntRect::endY () const {return y + height;}

bool IntRect::isZeroArea () const {return width == 0 || height == 0;}

bool IntRect::isIntersectsRect (const IntRect & rect) const
{
    return !(
        endX() <= rect.beginX() || rect.endX() <= beginX() 
        || endY() <= rect.beginY() || rect.endY() <= beginY()
    );
}

IntRect IntRect::getIntersectWithRect (const IntRect & rect) const
{
    if (isIntersectsRect(rect))
    {
        int i_begin_x = std::max(beginX(), rect.beginX());
        int i_end_x = std::min(endX(), rect.endX());       // Гарантировано >= i_begin_x 
        int i_begin_y = std::max(beginY(), rect.beginY());
        int i_end_y = std::min(endY(), rect.endY());       // Гарантировано >= i_begin_y 
        return IntRect{
            i_begin_x, 
            i_begin_y, 
            static_cast <unsigned int> (i_end_x - i_begin_x), 
            static_cast <unsigned int> (i_end_y - i_begin_y)
        };
    }
    else {
        return IntRect{};
    }
}

void init_bool_bit_map_from_image (BoolBitMap & bit_map, const std::string & image_file_name)
{
    // Данные в Image идут с левого верхнего угла. Сначала справа-налево, затем то же для строки ниже.
    // Координаты в битовой карте [x][y] с левого нижнего угла. Смещение y * width + x. 
    // На каждый пиксел 4 байта данных с альфа-каналом, и по 3 байта без него.
    // В битовой карте false будет обозначать свободное место, а true - занятое.
    
    auto image = new Image();
    
    if (image->initWithImageFile(image_file_name))
    {
        int width = image->getWidth();
        int height = image->getHeight();
        if (width <= 0 || height <= 0) throw std::runtime_error("init_bool_bit_map_from_image: bad size of image.");
        auto data = image->getData();
        
        if (image->getBitPerPixel() == 32 && image->hasAlpha())
        {
            bit_map.init(width, height);
            
            for (int x = 0; x < width; ++x)
                for (int y = 0; y < height; ++y)
                    bit_map._p[y * width + x] = *(data + (width * (height - y - 1) + x) * 4 + 3) != 0;
        }
        else if (image->getBitPerPixel() == 24 && !image->hasAlpha())
        {
            bit_map.init(width, height);
            
            auto pixel = data;
            for (int x = 0; x < width; ++x)
                for (int y = 0; y < height; ++y)
                {
                    pixel = data + (width * (height - y - 1) + x) * 3;
                    bit_map._p[y * width + x] = (*pixel == 0 && *(pixel + 1) == 0 && *(pixel + 2) == 0);
                }
        }
    }
    
    delete image;
}

/*Rect get_intersection_of_rects (const Rect & rect1, const Rect & rect2)
{
    if (rect1.intersectsRect(rect2))
    {
        float left_x = std::max(rect1.getMinX(), rect2.getMinX());
        float right_x = std::min(rect1.getMaxX(), rect2.getMaxX());
        float bottom_y = std::max(rect1.getMinY(), rect2.getMinY());
        float top_y = std::min(rect1.getMaxY(), rect2.getMaxY());
        return Rect(left_x, bottom_y, right_x - left_x, top_y - bottom_y);
    }
    else {
        return Rect(0.0f, 0.0f, 0.0f, 0.0f);
    }
}*/

bool is_intersect_two_bitmaps (
    const Vec2 & left_bottom_point_1, const BoolBitMap & bit_map_1,
    const Vec2 & left_bottom_point_2, const BoolBitMap & bit_map_2)
{
    if (bit_map_1.is_init() && bit_map_2.is_init())
    {
        IntRect int_bb_1 {
            static_cast <int> (left_bottom_point_1.x + 0.5f),
            static_cast <int> (left_bottom_point_1.y + 0.5f),
            bit_map_1._width,
            bit_map_1._height
        };
        IntRect int_bb_2 {
            static_cast <int> (left_bottom_point_2.x + 0.5f),
            static_cast <int> (left_bottom_point_2.y + 0.5f),
            bit_map_2._width,
            bit_map_2._height
        };
        IntRect intersect_rect = int_bb_1.getIntersectWithRect(int_bb_2);
        
        if (!intersect_rect.isZeroArea())
        {
            int dx1 = intersect_rect.x - int_bb_1.x;
            int dy1 = intersect_rect.y - int_bb_1.y;
            int dx2 = intersect_rect.x - int_bb_2.x;
            int dy2 = intersect_rect.y - int_bb_2.y;
            
            for (unsigned int x = 0; x < intersect_rect.width; ++x)
                for (unsigned int y = 0; y < intersect_rect.height; ++y)
                    if (bit_map_1.at(dx1 + x, dy1 + y) && bit_map_2.at(dx2 + x, dy2 + y))
                        return true;
        }
    }
    return false;
}

bool is_intersect_bitmap_and_rect (
    const Vec2 & left_bottom_map_point, const BoolBitMap & bit_map, 
    const Rect & rect)
{
    if (bit_map.is_init() && rect.size.width > 0.0f && rect.size.height > 0.0f)
    {
        IntRect int_bb_1 {
            static_cast <int> (left_bottom_map_point.x + 0.5f),
            static_cast <int> (left_bottom_map_point.y + 0.5f),
            bit_map._width,
            bit_map._height
        };
        IntRect int_bb_2 {
            static_cast <int> (rect.origin.x + 0.5f),
            static_cast <int> (rect.origin.y + 0.5f),
            static_cast <unsigned int> (rect.size.width),
            static_cast <unsigned int> (rect.size.height)
        };
        IntRect intersect_rect = int_bb_1.getIntersectWithRect(int_bb_2);
        
        if (!intersect_rect.isZeroArea())
        {
            int dx = intersect_rect.x - int_bb_1.x;
            int dy = intersect_rect.y - int_bb_1.y;
            
            for (unsigned int x = 0; x < intersect_rect.width; ++x)
                for (unsigned int y = 0; y < intersect_rect.height; ++y)
                    if (bit_map.at(dx + x, dy + y))
                        return true;
        }
    }
    return false;
}

bool is_intersect_bitmap_and_point (
    const Vec2 & left_bottom_map_point, const BoolBitMap & bit_map,
    const Vec2 & check_point)
{
    if (bit_map.is_init())
    {
        int x = static_cast <int> (check_point.x - left_bottom_map_point.x + 0.5f);
        int y = static_cast <int> (check_point.y - left_bottom_map_point.y + 0.5f);
        if (x >= 0 && x < static_cast <int> (bit_map._width) && y >= 0 && y < static_cast <int> (bit_map._height))
            return bit_map.at(x, y);
    }
    return false;
}

void draw_circle_on_bitmap (BoolBitMap & bit_map, int x, int y, unsigned int radius)
{
    if (radius > 0 && bit_map.is_init())
    {
        IntRect base_rect {0, 0, bit_map._width, bit_map._height};
        IntRect draw_rect {x - static_cast <int> (radius), y - static_cast <int> (radius), radius * 2 + 1};
        IntRect intersect_rect = base_rect.getIntersectWithRect(draw_rect);
        
        if (!intersect_rect.isZeroArea())
        {
            int begin_x = intersect_rect.beginX();
            int end_x = intersect_rect.endX();
            int begin_y = intersect_rect.beginY();
            int end_y = intersect_rect.endY();
            int squared_radius = radius * radius;
            
            for (int i = begin_x; i < end_x; ++i)
                for (int j = begin_y; j < end_y; ++j)
                    if ((x - i) * (x - i) + (y - j) * (y - j) <= squared_radius)
                        bit_map._p[j * bit_map._width + i] = true;
        }
    }
}

// только RGBA8888 - см. исходники метода Image::initWithRawData.
void write_image_from_bitmap (const BoolBitMap & bit_map, const std::string & image_full_file_name)
{
    if (bit_map._width > 0 && bit_map._height > 0)
    {
        unsigned int bytes_count = bit_map._width * bit_map._height * 4;
        unsigned char * buff = new unsigned char [bytes_count];
        
        for (unsigned int x = 0; x < bit_map._width; ++x)
            for (unsigned int y = 0; y < bit_map._height; ++y)
            {
                unsigned char * shift = buff + ((bit_map._height - y - 1) * bit_map._width + x) * 4;
                *shift = 0;
                *(shift + 1) = 0;
                *(shift + 2) = 0;
                *(shift + 3) = (bit_map._p[y * bit_map._width + x] ? 0xff : 0);
            }
        
        Image * img = new Image();
        img->initWithRawData(buff, bytes_count, bit_map._width, bit_map._height, 32);
        img->saveToFile(image_full_file_name, false);
        
        img->release();
        delete [] buff;
    }
}

SpriteFrame * create_sprite_frame (const std::string & fname)
{
    SpriteFrame * result = nullptr;
    Texture2D * pTexture = Director::getInstance()->getTextureCache()->addImage(fname);
    if (pTexture) {
        pTexture->setAliasTexParameters();
        Rect rect = Rect(0, 0, 0, 0);
        rect.size = pTexture->getContentSize();
        result = SpriteFrame::createWithTexture(pTexture, rect);
    }
    return result;
}

// Хотя используется sprintf, зато работает быстро. 
Animation * create_animation (
    const std::string & fname_template, 
    unsigned int frames_count, 
    float delay_per_unit,
    bool restore_orig_frame,
    unsigned int fname_max_lenght,
    int start_num_from,
    int first_frame_anim
) {
    ++fname_max_lenght;
    char * fname = new char[fname_max_lenght];
    Animation * result = Animation::create();
    int end_number = start_num_from + frames_count;

    if (first_frame_anim < start_num_from || first_frame_anim >= end_number)
        first_frame_anim = start_num_from;

    for (int i = first_frame_anim; i < end_number; ++i)
    {
        for (unsigned int j = 0; j < fname_max_lenght; ++j) fname[j] = '\0';
        std::sprintf(fname, fname_template.c_str(), i);
        result->addSpriteFrame(create_sprite_frame(fname));
    }
    for (int i = start_num_from; i < first_frame_anim; ++i)
    {
        for (unsigned int j = 0; j < fname_max_lenght; ++j) fname[j] = '\0';
        std::sprintf(fname, fname_template.c_str(), i);
        result->addSpriteFrame(create_sprite_frame(fname));
    }

    result->setDelayPerUnit(delay_per_unit);
    result->setRestoreOriginalFrame(restore_orig_frame);
    delete [] fname;
    return result;
}

Animation * create_animation (const AnimationParams & p)
{
    if (p.is_valid())
        return create_animation(
            p.fname_template, p.frames_count, p.delay_per_unit, p.restore_orig_frame,
            p.fname_max_lenght, p.start_num_from, p.first_frame_anim
        );
    else 
        return nullptr;
}

float rad_to_degree (float rad)
{
    return rad * (180.0f / 3.14159265f);
}

float degree_to_rad (float degree)
{
    return (degree / 180.0f) * 3.14159265f;
}

float get_angle_of_vector (Vec2 vec)
{
    vec.normalize();
    float angle = rad_to_degree(acos(vec.x));
    if (vec.y > 0.0f) angle = -angle;
    return angle;
}
