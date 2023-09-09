# PicoSSH1106GraphicsEngine
A thread safe graphics engine for the SH1106 SPI OLED LCD display using the Raspberry Pi Pico C++ SDK 

Example Usage:
 
Create a new class that inherits gui_element
GuiBarGraph.h
```
#ifndef _GUIBARGRAPH_H_
#define _GUIBARGRAPH_H_

#include "stdint.h"
#include "GuiElement.h"

class gui_bargraph : public gui_element
{
    public:
        gui_bargraph(uint16_t width, uint16_t height, uint16_t maxval);
        void setBarVal(int value);
        virtual void draw(std::shared_ptr<sh1106_frame> frameptr);

    private:
        uint16_t _max_bar_val;
        uint16_t _bar_val;
        uint16_t _bar_width;
        uint16_t _bar_height;
};

#endif
```
GuiBarGraph.cpp
```
#include "GuiBarGraph.h"

gui_bargraph::gui_bargraph(uint16_t width, uint16_t height, uint16_t max_bar_val): 
    gui_element(width,height,0,0),
    _max_bar_val(max_bar_val),
    _bar_width(width),
    _bar_height(height)
{
    // start bar graph at 50% full
    _bar_val = _max_bar_val / 2;
    
}

void gui_bargraph::setBarVal(int value)
{
    if(value > _max_bar_val || value < 0)
    {
        return;
    }

    _bar_val = value;
}

void gui_bargraph::draw(std::shared_ptr<sh1106_frame> frameptr)
{
    // create horizontal boarders
    for(int x = 0; x < _bar_width; x++)
    {
        frameptr->WritePixel(x + getPosX(), getPosY(), 1);
        frameptr->WritePixel(x + getPosX(), getPosY() + _bar_height - 1, 1);
    }
    // create vertical boarders
    for(int y = 0; y < _bar_height; y++)
    {
        frameptr->WritePixel(getPosX(), y + getPosY(), 1);
        frameptr->WritePixel(getPosX() + _bar_width - 1, y + getPosY(), 1);
    }

    uint16_t bar_pixel_loc = (_bar_val * _bar_width) /_max_bar_val;

    for(int x = 1; x < _bar_width - 1; x++)
    {
        for(int y = 1; y < _bar_height - 1; y++)
        {
            if(x <= bar_pixel_loc)
            {
                frameptr->WritePixel(x + getPosX(), y + getPosY(), 1);
            }
            else
            {
                frameptr->WritePixel(x + getPosX(), y + getPosY(), 0);
            }
        }
    }
}

```


Implementation in main

```
// create a gui_manager instance

gui_manager gui(60, TIMER_IRQ_0, 0, 8, 128, 64, SCLK_PIN, MOSI_PIN, CS_PIN, RST_PIN, DC_PIN);
    
// create shared pointer to a new box element so we can update parameters from the main thread

std::shared_ptr<gui_bargraph> bar1;
bar1.reset(new gui_bargraph(48,8,100));
bar1->setPosition(79,2);

// add box to gui_manager

gui.AddGuiElement(boxptr);

// start gui manager. now we are hands off! parameters of 
// boxelement can be changed on the fly from core0 while core1 
// handles all rendering and data transfer to the display at the 
// specified frame rate.

gui.StartGui();

int barval = 0;
int barmax = 100;
while(1)
{
    bar1->setBarVal(barval);
    barval = (barval + 1) % barmax;
    sleep_ms(100);
}
```
  

