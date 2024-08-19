cmake_minimum_required(VERSION 3.16)

idf_component_register(
SRCS "Utils.cpp"
INCLUDE_DIRS "include"
REQUIRES nvs_flash
)

project(Utils)
