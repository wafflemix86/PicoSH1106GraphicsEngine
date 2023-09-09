#include "GuiElement.h"

gui_element::gui_element(uint16_t element_width, uint16_t element_height, uint16_t x_pos, uint16_t y_pos):
    _element_width(element_width),
    _element_height(element_height),
    _x_pos(x_pos),
    _y_pos(y_pos)
{
}

void gui_element::setSize(uint16_t x, uint16_t y)
{
    _element_width = x;
    _element_height = y;
}

void gui_element::draw(std::shared_ptr<sh1106_frame> frameptr)
{
    /// this function does nothing in the base class and depends on 
    /// the derived class 
    return;
}