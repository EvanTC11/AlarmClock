//
// Created by happy on 19/08/2024.
//

#include "GPIO.h"

#include "esp_log.h"

namespace ESP32::GPIO
{
    Pin::Pin(gpio_num_t number, gpio_mode_t mode)
            : m_Number(number)
    {
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = mode;
        io_conf.pin_bit_mask = (1ULL << m_Number);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        
        if (gpio_config(&io_conf) != ESP_OK)
        {
            ESP_LOGE(s_TAG, "Failed to configure GPIO %d", m_Number);
        }
    }

    Pin::~Pin()
    {
        if (gpio_reset_pin(m_Number) != ESP_OK)
        {
            ESP_LOGE(s_TAG, "Failed to reset GPIO %d", m_Number);
        }
    }

    void Pin::Set(uint32_t level)
    {
        if (gpio_set_level(m_Number, level) != ESP_OK)
        {
            ESP_LOGE(s_TAG, "Failed to set level for GPIO %d", m_Number);
        }
    }
}