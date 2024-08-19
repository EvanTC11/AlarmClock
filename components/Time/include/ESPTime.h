//
// Created by happy on 18/06/2024.
//

#pragma once

#include <time.h>
#include "esp_sntp.h"

namespace ESP32
{
    class Time {
    public:
        explicit Time(const char *timeZone = "UTC", bool setup = true);
        ~Time();

        void Stop();

        void Setup();
        const struct tm& Get();
    private:
        time_t m_Now;
        struct tm m_TimeInfo;

        char* m_TimeZone;
        bool m_Set;
    };
}
