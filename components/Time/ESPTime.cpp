//
// Created by happy on 18/06/2024.
//

#include <esp_log.h>
#include <esp_netif_sntp.h>
#include "ESPTime.h"

namespace ESP32
{
    Time::Time(const char* timeZone, bool setup)
        : m_Now(0), m_TimeInfo({}), m_TimeZone((char*)timeZone), m_Set(false)
    {
        if (setup)
            Setup();
    }

    Time::~Time()
    {
        Stop();
    }

    void Time::Stop()
    {
        if (!m_Set)
            return;

        esp_sntp_stop();
        m_Set = false;
    }

    void Time::Setup()
    {
        if (m_Set)
            return;

        setenv("TZ", m_TimeZone, 1);
        tzset();

        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "pool.ntp.org");
        esp_sntp_init();

        int retryCount = 0;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retryCount++ < 10)
        {
            ESP_LOGI("Time", "Waiting for system time to be set...");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        time(&m_Now);
        localtime_r(&m_Now, &m_TimeInfo);

        if (m_TimeInfo.tm_year < (2016 - 1900))
        {
            ESP_LOGE("Time", "Time failed to set");
            Setup();
            time(&m_Now);
        }

        m_Set = true;
    }

    const struct tm& Time::Get()
    {
        time(&m_Now);
        localtime_r(&m_Now, &m_TimeInfo);
        return m_TimeInfo;
    }
}