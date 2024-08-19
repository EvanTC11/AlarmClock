//
// Created by happy on 19/08/2024.
//

#pragma once

#include "driver/gpio.h"

namespace ESP32::GPIO
{
    static const char* s_TAG = "GPIO";
    
    class Pin
    {
    public:
        Pin(gpio_num_t number, gpio_mode_t mode);
        ~Pin();
        
        void Set(uint32_t level);
        int GetValue() const { return gpio_get_level(m_Number); }
        
    private:
        gpio_num_t m_Number;
    };

} // GPIO