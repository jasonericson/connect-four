cmake_minimum_required(VERSION 3.10)

project(ConnectFour)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_executable(ConnectFour main.cpp)

target_link_libraries(ConnectFour PRIVATE SDL3::SDL3)
