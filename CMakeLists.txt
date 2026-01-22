cmake_minimum_required(VERSION 3.16.0)
project(CTS256A_SP0256 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(CTS256A-AL2)
add_subdirectory(SP0256)
add_subdirectory(cts_eprom)
