cmake_minimum_required(VERSION 3.5)

project(mavsdk_setup_udp_remote_issue LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(MAVSDK REQUIRED)

add_definitions("-std=c++11 -Wall -Wextra -Werror")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(issue_demo main.cpp)

target_link_libraries(issue_demo
    MAVSDK::mavsdk
    MAVSDK::mavsdk_mavlink_passthrough
    ${CMAKE_THREAD_LIBS_INIT}
)
