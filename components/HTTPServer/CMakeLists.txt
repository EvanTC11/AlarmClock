cmake_minimum_required(VERSION 3.16)

idf_component_register(
SRCS "HTTPServer.cpp"
INCLUDE_DIRS "include"
REQUIRES esp_http_server json
)

project(HTTPServer)
