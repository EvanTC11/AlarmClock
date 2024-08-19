//
// Created by happy on 17/06/2024.
//

#pragma once

#include <vector>

#include "esp_http_server.h"

namespace ESP32::HTTP
{
    class HTTPServer
    {
    public:
        HTTPServer(const std::vector<httpd_uri>& endpoints = {}, httpd_config_t config = HTTPD_DEFAULT_CONFIG())
                : m_TAG("HTTPServer"), m_server(nullptr), m_config(config), m_endpoints(endpoints)
        {}

        ~HTTPServer() { Stop(); }

        void AddEndpoint(const char* uri, httpd_method_t method, esp_err_t(*handler)(httpd_req_t *r), void* user_ctx = nullptr);

        [[nodiscard]]
        bool IsRunning() const { return m_server != nullptr; }

        void Start();
        void Stop() const;
    private:
        const char* m_TAG;

        httpd_handle_t m_server;
        httpd_config_t m_config;
        std::vector<httpd_uri_t> m_endpoints;
    };

};