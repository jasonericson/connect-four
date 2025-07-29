cmake_minimum_required(VERSION 3.10)

project(ConnectFour)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_executable(ConnectFour main.cpp core.cpp)
target_link_libraries(ConnectFour PRIVATE SDL3::SDL3)
target_include_directories(ConnectFour PRIVATE SDL/include/SDL3)

add_executable(ConnectFour-Profile profile.cpp core.cpp)
target_link_libraries(ConnectFour-Profile PRIVATE SDL3::SDL3)
target_include_directories(ConnectFour-Profile PRIVATE SDL/include/SDL3)
