#include "GuiManager.h"
#include <cstdlib>
#include "stdio.h"


static gui_manager* gui_manager_instance;

static void gui_manager_irq_handler()
{
    gui_manager_instance->_handleTimerIRQ();
};

void gui_manager::InstallIRQHandler(uint timer_irq_num)
{
    gui_manager_instance = this;
    irq_set_exclusive_handler(timer_irq_num, gui_manager_irq_handler);

}

void gui_manager::_handleTimerIRQ(void) {
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << _timer_alarm_num);
    uint32_t frame_delay = 1000000/_fps;
    alarm_in_us(frame_delay);         

    _ssh1106->setBuffer(_frame_buffers[_frame_read_idx]->Frame());
    _ssh1106->display();
    _frame_read_idx = (_frame_read_idx + 1) % _num_buffers;
    // static int count = 0;
    // printf("hello %d",count++);

}

void gui_manager::alarm_in_us(uint32_t delay_us) {

    // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + delay_us;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[_timer_alarm_num] = (uint32_t) target;
}


gui_manager::gui_manager(uint32_t fps, uint timer_irq_num, uint timer_alarm_num, uint8_t num_buffers, uint16_t frame_width, uint16_t frame_height, uint8_t sclk_pin, uint8_t mosi_pin, uint8_t cs_pin, uint8_t rst_pin, uint8_t dc_pin):
    _timer_irq_num(timer_irq_num),
    _timer_alarm_num(timer_alarm_num),
    _num_buffers(num_buffers),
    _frame_width(frame_width),
    _frame_height(frame_height),
    _fps(fps)
{
    if(num_buffers == 0 || frame_width == 0 || frame_height == 0)
    {
        return;
    }
    _frame_buffers.resize(num_buffers);
    
    
    for(int i = 0; i < num_buffers; i++)
    {
        _frame_buffers[i].reset(new sh1106_frame(frame_width,frame_height));
    }
    
    _frame_write_idx = _num_buffers/2;
    _frame_read_idx = _num_buffers - 1;

    _ssh1106.reset(new SSH1106(spi0,sclk_pin,mosi_pin,cs_pin,rst_pin,dc_pin));
}

void gui_manager::StartGui()
{
    _ssh1106->init();

    // clear all display buffers to zeros
    for(int i = 0; i < _num_buffers; i++)
    {
        _ssh1106->setBuffer(_frame_buffers[i]->Frame());
        _ssh1106->clear();
    }
    _ssh1106->display();

    // // Enable interrupt in block and at processor
    // hw_set_bits(&timer_hw->inte, 1u << _timer_alarm_num);
    // // Set irq handler for alarm irq
    // InstallIRQHandler(_timer_irq_num);
    // // Enable the alarm irq
    // irq_set_enabled(_timer_irq_num, true);

    gui_manager_instance = this;
    // kick off gui rendering and displaying on second cpu
    multicore_launch_core1([](){gui_manager_instance->RenderFrame();});

}

void gui_manager::AddGuiElement(std::shared_ptr<gui_element> element)
{
    _gui_elements.push_back(element);
}

void gui_manager::RenderFrame()
{


    // Enable interrupt in block and at processor
    hw_set_bits(&timer_hw->inte, 1u << _timer_alarm_num);
    irq_set_exclusive_handler(_timer_irq_num, gui_manager_irq_handler);
    // Enable the alarm irq
    irq_set_enabled(_timer_irq_num, true);
    // start display loop
    uint32_t delay_ms = 1000000 / _fps;
    alarm_in_us(delay_ms);

    while(1)
    {
        // hold off rendering until read index advances
        // printf("write_buffer: %d read_buffer: %d\r\n",_frame_write_idx,_frame_read_idx);
        while((_frame_write_idx + 1) % _num_buffers == _frame_read_idx){}

        _frame_buffers[_frame_write_idx]->Clear();
        for(std::shared_ptr<gui_element> e : _gui_elements)
        {
            // uint16_t elementwidth = e->getWidth();
            // uint16_t elementheight = e->getHeight();
            // const uint8_t* elementdata = e->getElementData();
            // uint16_t x_offset = e->getPosX();
            // uint16_t y_offset = e->getPosY();

            e->draw(_frame_buffers[_frame_write_idx]);

            // for(int x = 0; x < elementwidth; x++)
            // {
            //     for(int y = 0; y < elementheight; y++)
            //     {
            //         _frame_buffers[_frame_write_idx]->WritePixel(x + x_offset, y + y_offset, elementdata[x + y*elementwidth]);
            //     }
            // }
        }

        _frame_write_idx = (_frame_write_idx + 1) % _num_buffers;
    }

}

void gui_manager::DisplayFrame()
{
    _ssh1106->setBuffer(_frame_buffers[_frame_read_idx]->Frame());
    _ssh1106->display();
    _frame_read_idx = (_frame_read_idx + 1) % _num_buffers;
}