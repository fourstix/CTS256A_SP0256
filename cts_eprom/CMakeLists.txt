cmake_minimum_required(VERSION 3.16.0)
project(cts_eprom VERSION 0.1.0 LANGUAGES CXX)

set (SOURCE_FILES
    main.cpp
    allophone.cpp
    cts_exception.cpp
    exception_rom.cpp
)

add_executable(cts_eprom ${SOURCE_FILES})
