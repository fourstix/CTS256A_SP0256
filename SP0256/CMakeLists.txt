cmake_minimum_required(VERSION 3.16.0)
project(sp0256 VERSION 0.1.0 LANGUAGES CXX)

set (SOURCE_FILES
    main.cpp
    audio.cpp
    sp0256_012.cpp
    sp0256_al2.cpp
    sp0256.cpp
    SystemClock.cpp
    WaveWriter.cpp
    sound.cpp
)

add_executable(sp0256 ${SOURCE_FILES})
