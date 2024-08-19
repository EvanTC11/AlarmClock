#include "Wifi.h"
#include "Utils.h"
#include "HTTPServer.h"
#include "ESPTime.h"
#include "GPIO.h"

#include "esp_log.h"

#include "freertos/task.h"
#include "cJSON.h"

extern "C" void app_main(void);

static auto s_TAG = "elodie-birthday";

static struct
{
    uint32_t days, hours, minutes, seconds;
    bool set;
} s_Alarm;

static ESP32::GPIO::Pin s_Led(GPIO_NUM_2, GPIO_MODE_OUTPUT);

static ESP32::WiFi::WiFiConfig s_WifiConfig((uint8_t*)("KC and JoJo"), (uint8_t*)("Ev@n_090111"), {ESP32::WiFi::handle_event_default, &s_WifiConfig });
static ESP32::Time s_Time("UTC-1", false);

static ESP32::HTTP::HTTPServer s_Server;

static esp_err_t alarm_handler(httpd_req_t* req)
{
    char content[1024];
    int ret = httpd_req_recv(req, content, req->content_len);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }

        return ESP_FAIL;
    }

    cJSON* json = cJSON_Parse(content);
    if (!json)
    {
        cJSON_Delete(json);

        const char* response = "<h1>Invalid Request</h1>";
        httpd_resp_send(req, response, (int)strlen(response));

        return ESP_OK;
    }
    
    cJSON* resetJson = cJSON_GetObjectItem(json, "reset");
    if ((!resetJson || !cJSON_IsBool(resetJson)) && s_Alarm.set)
    {
        cJSON_Delete(json);

        const char* response = "<h1>Alarm already set, and no instruction to reset it</h1>";
        httpd_resp_send(req, response, (int)strlen(response));

        return ESP_OK;
    }

    cJSON* hourJson = cJSON_GetObjectItem(json, "hour");
    if (!cJSON_IsNumber(hourJson))
    {
        cJSON_Delete(json);

        const char* response = "<h1>Invalid Request</h1>";
        httpd_resp_send(req, response, (int)strlen(response));

        return ESP_OK;
    }

    cJSON* minuteJson = cJSON_GetObjectItem(json, "minute");
    if (!cJSON_IsNumber(minuteJson))
    {
        cJSON_Delete(json);

        const char* response = "<h1>Invalid Request</h1>";
        httpd_resp_send(req, response, (int)strlen(response));

        return ESP_OK;
    }

    cJSON* secondJson = cJSON_GetObjectItem(json, "second");
    if (!cJSON_IsNumber(secondJson))
    {
        cJSON_Delete(json);

        const char* response = "<h1>Invalid Request</h1>";
        httpd_resp_send(req, response, (int)strlen(response));

        return ESP_OK;
    }

    int hour = hourJson->valueint;
    int minute = minuteJson->valueint;
    int second = secondJson->valueint;

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59)
    {
        cJSON_Delete(json);

        const char* response = "<h1>Invalid Request</h1>";
        httpd_resp_send(req, response, (int)strlen(response));

        return ESP_OK;
    }

    const auto& time = s_Time.Get();
    if (hour < time.tm_hour || (hour == time.tm_hour && minute < time.tm_min) || (hour == time.tm_hour && minute == time.tm_min && second < time.tm_sec)) // If the alarm is set for a time in the past
        s_Alarm.days = 1;
    else
        s_Alarm.days = 0;

    s_Alarm.set = true;
    s_Alarm.hours = hour;
    s_Alarm.minutes = minute;
    s_Alarm.seconds = second;

    const char* response = "<h1>Alarm set successfully</h1>";
    httpd_resp_send(req, response, (int)strlen(response));

    ESP_LOGI(s_TAG, "Alarm set for %d:%d:%d", hour, minute, second);

    return ESP_OK;
}

void app_main(void)
{
    s_Alarm =
    {
        .days = 0,
        .hours = 0,
        .minutes = 0,
        .seconds = 0,
        .set = false
    };
    
    s_Led.Set(1);

    ESP32::Utils::InitNVS();
    ESP32::WiFi::Connect(s_WifiConfig);

    s_Time.Setup();

    s_Server.Start();
    s_Server.AddEndpoint("/set", HTTP_POST, alarm_handler);
    
    s_Led.Set(0);

    while (true)
    {
        const auto &time = s_Time.Get();
        ESP_LOGI("Time: ", "%d:%d:%d Waiting for time: %lu:%lu:%lu", time.tm_hour, time.tm_min, time.tm_sec, s_Alarm.hours, s_Alarm.minutes, s_Alarm.seconds);

        if (s_Alarm.set)
        { 
            if (time.tm_hour == s_Alarm.hours && time.tm_min == s_Alarm.minutes && time.tm_sec == s_Alarm.seconds)
            {
                ESP_LOGI(s_TAG, "Alarm triggered!");
                s_Alarm.set = false;
                break;
            }

            if (s_Alarm.days > 0)
                s_Alarm.days --;
        }

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    s_Server.Stop();
    s_Time.Stop();
    ESP32::WiFi::Disconnect(s_WifiConfig.connection_info);
    
    for (int i = 0; i < 50; i++)
    {
        s_Led.Set(1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        s_Led.Set(0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}