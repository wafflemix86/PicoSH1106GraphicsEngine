#ifndef _GUIELEMENT_H_
#define _GUIELEMENT_H_

#include "stdint.h"
#include "sh1106_frame.h"
#include <vector>
#include <memory>

class gui_element
{
    public:
        gui_element(uint16_t element_width, uint16_t element_height, uint16_t x_pos = 0, uint16_t y_pos= 0);
        void setPosition(uint16_t x, uint16_t y){_x_pos = x; _y_pos = y;}
        void setSize(uint16_t x, uint16_t y);
        uint16_t getPosX(){return _x_pos;}
        uint16_t getPosY(){return _y_pos;}
        uint16_t getWidth(){ return _element_width; }
        uint16_t getHeight(){ return _element_height; }
        virtual void draw(std::shared_ptr<sh1106_frame> frameptr);

    private:
        uint16_t _x_pos;
        uint16_t _y_pos;
        uint16_t _element_width;
        uint16_t _element_height;
};




#endif