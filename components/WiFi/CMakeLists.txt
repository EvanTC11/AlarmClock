cmake_minimum_required(VERSION 3.16)

idf_component_register(
SRCS "WiFi.cpp"
INCLUDE_DIRS "include"
REQUIRES esp_wifi nvs_flash 
)

project(WiFi)
