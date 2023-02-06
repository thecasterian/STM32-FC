cmake_minimum_required(VERSION 3.18)

project("STM32-FC Ground Control")

set(CMAKE_AUTOUIC ON)

set(TARGET "ground")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

set(QT_MIN_VERSION "5.15.2")
set(QWT_MIN_VERSION "6.1.4")

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets SerialPort)
find_package(Qwt ${QWT_MIN_VERSION} REQUIRED)

add_executable(${TARGET}
    Src/command_tab.cpp
    Src/main_window.cpp
    Src/main.cpp
    Src/plotting_tab.cpp
    Src/port_setting.cpp
)

target_include_directories(${TARGET} PUBLIC
    "Inc"
    ${QWT_INCLUDE_DIR}
)

target_link_libraries(${TARGET}
    Qt5::Core
    Qt5::Widgets
    Qt5::SerialPort
    ${QWT_LIBRARY}
)