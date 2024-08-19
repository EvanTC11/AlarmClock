//
// Created by happy on 17/06/2024.
//

#include "HTTPServer.h"

#include "esp_log.h"

namespace ESP32::HTTP
{
    void HTTPServer::AddEndpoint(const char *uri, httpd_method_t method, esp_err_t(*handler)(httpd_req_t *r), void *user_ctx)
    {
        m_endpoints.push_back({uri, method, handler, user_ctx});
        httpd_register_uri_handler(m_server, &m_endpoints.back());
    }


    void HTTPServer::Start()
    {
        if (httpd_start(&m_server, &m_config) != ESP_OK)
        {
            ESP_LOGE(m_TAG, "Failed to create HTTP Server");
            return;
        }

        if (!m_endpoints.empty())
        {
            for (auto& endpoint : m_endpoints)
            {
                httpd_register_uri_handler(m_server, &endpoint);
            }
        }

        ESP_LOGI(m_TAG, "HTTP Server started");
    }

    void HTTPServer::Stop() const
    {
        if (IsRunning())
        {
            // for (const auto&[uri, method, handler, user_ctx] : m_endpoints)
            // {
            //     httpd_unregister_uri_handler((httpd_handle_t)handler, uri, method);
            // }

            httpd_stop(m_server);
            ESP_LOGI(m_TAG, "HTTP Server stopped");
        }
    }
} // ESP32