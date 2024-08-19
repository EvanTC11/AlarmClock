//
// Created by happy on 17/06/2024.
//

#pragma once

#include "esp_wifi_types.h"

#include "esp_event.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

namespace ESP32::WiFi
{
    static const char* s_TAG = "WifiConnection";

    void handle_event_default(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    typedef struct ConnectionInfo_t
    {
        EventGroupHandle_t wifi_event_group;

        esp_event_handler_instance_t got_id;
        esp_event_handler_instance_t got_ip;
    } ConnectionInfo;

    typedef struct ConnectionHandler_t
    {
        esp_event_handler_t func;
        void* arg;
    } ConnectionHandler;

    typedef struct WiFiConfig_t
    {
        WiFiConfig_t(const uint8_t* ssid, const uint8_t* password, ConnectionHandler connection_handler = {nullptr, nullptr})
                : ssid(ssid),
                  password(password),
                  retry_count(0),
                  connection_handler(connection_handler)
        {
        }

        const uint8_t *ssid, *password;
        int retry_count;

        ConnectionHandler connection_handler;
        ConnectionInfo connection_info;

    } WiFiConfig;

    void Connect(WiFiConfig& config);
    void Disconnect(const ConnectionInfo& config);

    bool Connected(const EventGroupHandle_t& wifi_event_group);
}
