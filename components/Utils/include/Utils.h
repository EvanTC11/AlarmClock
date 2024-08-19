//
// Created by happy on 17/06/2024.
//

#pragma once

namespace ESP32::Utils
{
    // template <typename T>
    // class Singleton
    // {
    // public:
    //     static T& GetInstance()
    //     {
    //         static T s_instance;
    //         return s_instance;
    //     }
    //
    //     Singleton(const Singleton&) = delete;
    //     Singleton(const Singleton&&) = delete;
    //     Singleton& operator=(const Singleton&) = delete;
    //
    // protected:
    //     Singleton() = default;
    //     ~Singleton() = default;
    // };

    void InitNVS();
}