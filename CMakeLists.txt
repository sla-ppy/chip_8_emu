cmake_minimum_required(VERSION 3.22)
project(chip_8_emu)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip_8_emu src/main.cpp src/Chip8.cpp)
