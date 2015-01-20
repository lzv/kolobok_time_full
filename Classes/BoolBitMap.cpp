#include "BoolBitMap.h"
#include <stdexcept>

BoolBitMap::BoolBitMap () : _p(nullptr), _width(0), _height(0) {}

BoolBitMap::~BoolBitMap ()
{
    if (_p != nullptr) delete [] _p;
}

bool BoolBitMap::is_init () const
{
    return _p != nullptr;
}

void BoolBitMap::init (unsigned int width, unsigned int height, bool init_value)
{
    if (width == 0 || height == 0) throw std::runtime_error("Init BoolBitMap with 0 size.");
    if (_p != nullptr) throw std::runtime_error("Init already initialized BoolBitMap.");
    _width = width;
    _height = height;
    unsigned int count = _width * _height;
    _p = new bool [count];
    for (unsigned int i = 0; i < count; ++i) _p[i] = init_value;
}

void BoolBitMap::copy_from (const BoolBitMap & obj)
{
    if (_p != nullptr) delete [] _p;
    _width = obj._width;
    _height = obj._height;
    _p = obj._p;
    if (_p != nullptr)
    {
        unsigned int count = _width * _height;
        _p = new bool [count];
        for (unsigned int i = 0; i < count; ++i) _p[i] = obj._p[i];
    }
}

bool BoolBitMap::at (unsigned int x, unsigned int y) const
{
    return _p[y * _width + x];
}

void BoolBitMap::set (unsigned int x, unsigned int y, bool value)
{
    _p[y * _width + x] = value;
}

bool BoolBitMap::is_inside (int x, int y)
{
    return x >= 0 && x < static_cast <int> (_width) && y >= 0 && y < static_cast <int> (_height);
}
