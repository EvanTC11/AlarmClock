//
// Created by happy on 17/06/2024.
//

#include <cstring>
#include "WiFi.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

namespace ESP32::WiFi
{
    void handle_event_default
            (
                void* arg,
                esp_event_base_t event_base,
                int32_t event_id,
                void* event_data
            )

    {
        auto wifi_config = static_cast<ESP32::WiFi::WiFiConfig*>(arg);

        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }

        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            if (wifi_config->retry_count < 10)
            {
                esp_wifi_connect();
                wifi_config->retry_count++;

                ESP_LOGI(s_TAG, "Reattempting to connect to Wifi AP");
            }

            else
            {
                xEventGroupSetBits(wifi_config->connection_info.wifi_event_group, WIFI_FAIL_BIT);
            }

            ESP_LOGW(s_TAG, "Connection to the Wifi AP failed");
        }

        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            const auto* event = static_cast<ip_event_got_ip_t*>(event_data);
            ESP_LOGI(s_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

            wifi_config->retry_count = 0;

            xEventGroupSetBits(wifi_config->connection_info.wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }

    void Connect(WiFiConfig& config)
    {
        ESP_LOGI(s_TAG, "SSID: %s", config.ssid);
        ESP_LOGI(s_TAG, "PASSWORD %s", config.password);

        config.connection_info.wifi_event_group = xEventGroupCreate();

        ESP_ERROR_CHECK(esp_netif_init());

        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        ESP_ERROR_CHECK(
                esp_event_handler_instance_register(
                        WIFI_EVENT,
                        ESP_EVENT_ANY_ID,
                        config.connection_handler.func,
                        config.connection_handler.arg,
                        &config.connection_info.got_id
                ));

        ESP_ERROR_CHECK(
                esp_event_handler_instance_register(
                        IP_EVENT,
                        IP_EVENT_STA_GOT_IP,
                        config.connection_handler.func,
                        config.connection_handler.arg,
                        &config.connection_info.got_ip
                ));

        wifi_config_t wifi_cfg = {};
        memcpy(wifi_cfg.sta.ssid, config.ssid, sizeof(wifi_cfg.sta.ssid));
        memcpy(wifi_cfg.sta.password, config.password, sizeof(wifi_cfg.sta.password));

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
        ESP_ERROR_CHECK(esp_wifi_start());

        ESP_LOGI(s_TAG, "Wifi initialised");

        if (Connected(config.connection_info.wifi_event_group))
        {
            ESP_LOGI(s_TAG, "Connected to the internet!");
            return;
        }

        ESP_LOGE(s_TAG, "Failed to connect to the internet");
    }

    void Disconnect(const ConnectionInfo& config)
    {
        ESP_ERROR_CHECK(
                esp_event_handler_instance_unregister(
                        WIFI_EVENT,
                        ESP_EVENT_ANY_ID,
                        config.got_id
                )
        );

        ESP_ERROR_CHECK(
                esp_event_handler_instance_unregister(
                        IP_EVENT,
                        IP_EVENT_STA_GOT_IP,
                        config.got_ip
                )
        );

        esp_err_t ret = esp_wifi_disconnect();
        if (ret != ESP_OK)
        {
            ESP_LOGE(s_TAG, "Failed to disconnect from wifi");
            return;
        }

        ESP_LOGI(s_TAG, "Successfully disconnected from wifi!");
    }

    bool Connected(const EventGroupHandle_t& wifi_event_group)
    {
        if (EventBits_t bits = xEventGroupWaitBits(
                    wifi_event_group,
                    WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                    pdFALSE,
                    pdFALSE,
                    portMAX_DELAY
            );

                bits & WIFI_CONNECTED_BIT)
        {
            return true;
        }

        return false;
    }
}