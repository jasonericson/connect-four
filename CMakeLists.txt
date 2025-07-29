cmake_minimum_required(VERSION 3.10)

project(ConnectFour)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_executable(ConnectFour main.cpp core.cpp)

target_include_directories(ConnectFour PRIVATE SDL/include/SDL3)

target_link_libraries(ConnectFour PRIVATE SDL3::SDL3)
