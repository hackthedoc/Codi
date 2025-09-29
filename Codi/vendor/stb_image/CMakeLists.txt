cmake_minimum_required(VERSION 3.16)

project(stb LANGUAGES CXX)

add_library(stb STATIC
    stb_image.h
    stb_image.cpp
)

target_include_directories(stb
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
