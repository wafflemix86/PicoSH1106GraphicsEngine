# PicoSSH1106GraphicsEngine
A thread safe graphics engine for the SH1106 SPI OLED LCD display using the Raspberry Pi Pico C++ SDK
Example Usage:
 
Create a new class that inherits gui_element
```
#ifndef _boxelement_h_
#define _boxelement_h_
class boxelement : public gui_element
{
    public:
    boxelement(int width, int height, int x_pos, int y_pos) : 
    	gui_element(width,height,x_pos,y_pos),
    	_width(width),
    	_height(height),
    {
    }
    
    void draw(std::shared_ptr<sh1106_frame> frameptr)
    {
    	// draw horizontal lines
        for(int x = 0; x < _width; x++)
        {
            frameptr->WritePixel(x + getPosX() , getPosY(), 1);
            frameptr->WritePixel(x + getPosX() , _height + getPosY(), 1);
        }
        
        // draw vertical lines
        for(int y = 0; y < _height; y++)
        {
            frameptr->WritePixel(getPosX() , y + getPosY(), 1);
            frameptr->WritePixel(_width + getPosX() , y + getPosY());
        }
    }
    private:
    	int _width;
    	int _height;
}
#endif
```

Implementation in main

```
// create a gui_manager instance
gui_manager mygui(fps, timer_irq_num, timer_alarm_num, num_buffers, frame_width, frame_height, sclk_pin, mosi_pin, cs_pin, rst_pin, dc_pin);

// create instance of custom gui element and shared pointer to it so we can update parameters from the main thread
boxelement mybox(40,20,32,32);
std::shared_ptr<boxelement> boxptr;

// add box to gui_manager
mygui.AddGuiElement(boxptr);

// start gui manager. now we are hands off! parameters of boxelement can be changed on the fly from core0 while core1 handles all rendering and data transfer to the display.
mygui.StartGui();
 
```
  

