#ifndef _GUIMANAGER_H_
#define _GUIMANAGER_H_

#include <memory>
#include <vector>
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "SSH1106_SPI_Lite.h"
#include "GuiElement.h"
#include "sh1106_frame.h"

class gui_manager
{
    public:
        gui_manager(uint32_t fps, uint timer_irq_num, uint timer_alarm_num, uint8_t num_buffers, uint16_t frame_width, uint16_t frame_height, uint8_t sclk_pin, uint8_t mosi_pin, uint8_t cs_pin, uint8_t rst_pin, uint8_t dc_pin);
        void StartGui();
        void AddGuiElement(std::shared_ptr<gui_element> element);
        void RenderFrame();
        void DisplayFrame();
        void _handleTimerIRQ();
    private:
        std::vector<std::shared_ptr<sh1106_frame>> _frame_buffers;
        std::vector<std::shared_ptr<gui_element>> _gui_elements;
        std::unique_ptr<SSH1106> _ssh1106;
        uint8_t _num_buffers;
        uint16_t _frame_width;
        uint16_t _frame_height;
        uint32_t _timer_irq_num;
        uint32_t _timer_alarm_num;
        uint32_t _fps;
        uint32_t _frame_write_idx;
        uint32_t _frame_read_idx;
        void InstallIRQHandler(uint timer_irq_num);
        void alarm_in_us(uint32_t delay_us);
};

#endif