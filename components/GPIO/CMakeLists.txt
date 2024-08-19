cmake_minimum_required(VERSION 3.16)

idf_component_register(
SRCS "GPIO.cpp"
INCLUDE_DIRS "include"
REQUIRES driver
)

project(GPIO)
