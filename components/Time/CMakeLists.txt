cmake_minimum_required(VERSION 3.16)

idf_component_register(
SRCS "ESPTime.cpp"
INCLUDE_DIRS "include"
REQUIRES esp_netif
)

project(Time)
