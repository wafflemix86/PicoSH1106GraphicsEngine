#ifndef _sh1106_frame_h
#define _sh1106_frame_h

#include "stdint.h"
#include <vector>
#include <memory>

class sh1106_frame
{
    public:
    sh1106_frame(int width_px, int height_px):
    _width(width_px),
    _height(height_px)
    {
        _frame.reset(new uint8_t[width_px*height_px/8]);
    }
    void WritePixel(uint8_t x, uint8_t y, uint8_t val)
    {
        switch (val) 
        {
        case 0: _frame[x+ (y >> 3)*_width] &= ~(1 << (y&7)); break; //black
        case 1: _frame[x+ (y >> 3)*_width] |=  (1 << (y&7)); break; //white
        case 2: _frame[x+ (y >> 3)*_width] ^=  (1 << (y&7)); break; //invert
        }
    }
    void Clear()
    {
        for(uint32_t i=0; i < _width*_height/8; i++)
        {
            _frame[i] = 0;
        }
    }
    uint8_t* Frame(){ return _frame.get(); }
    
    private:
    uint32_t _width;
    uint32_t _height;
    std::unique_ptr<uint8_t[]> _frame;

};


#endif