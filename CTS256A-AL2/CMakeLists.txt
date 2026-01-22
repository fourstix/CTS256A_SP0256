cmake_minimum_required(VERSION 3.16.0)
project(cts256a-al2 VERSION 0.1.0 LANGUAGES CXX)

set (SOURCE_FILES
    main.cpp
    ConIOConsole.cpp
    ConsoleDebugger.cpp
    CTS256A_AL2.cpp
    disas7000.cpp
    mem7000.cpp
    SystemConsole.cpp
    TMS7000CPU.cpp
    TMS7000DebugHelper.cpp
    TMS7000Disassembler.cpp
)

add_executable(cts256a-al2 ${SOURCE_FILES})
