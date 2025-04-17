cmake_minimum_required(VERSION 3.23)

# ======================================
# General section
# --------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -w -O2 -std=c++17 -pipe -Wall -Wextra")

# ======================================
# Project-specific
# --------------------------------------
project(Img2FourierGUI
    VERSION 1.0.0
    DESCRIPTION "Image Fourier Transform visualizer"
    LANGUAGES CXX)

add_executable(Img2Fourier src/img2fft_gui.cpp)

target_include_directories(Img2Fourier PUBLIC
    "./"
    "./include"
    "./external"
    "./external/imgui")