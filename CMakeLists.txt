cmake_minimum_required(VERSION 3.10)

project(ConnectFour)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_executable(ConnectFour main.cpp core.cpp helpers.cpp)
target_link_libraries(ConnectFour PRIVATE SDL3::SDL3)
target_include_directories(ConnectFour PRIVATE SDL/include/SDL3)
target_compile_options(ConnectFour PRIVATE -Wunused-variable)

add_executable(ConnectFour-Profile profile.cpp core.cpp helpers.cpp)
target_link_libraries(ConnectFour-Profile PRIVATE SDL3::SDL3)
target_include_directories(ConnectFour-Profile PRIVATE SDL/include/SDL3)

enable_testing()

set(TEST_LIST
    print_board
    win_horizontal
    win_vertical
    win_diagonal_bl-tr
    win_diagonal_tl-br
    win_none
)

foreach(TEST_NAME ${TEST_LIST})
    add_executable(${TEST_NAME} tests/${TEST_NAME}.cpp core.cpp tests/win_test.cpp helpers.cpp)
    target_link_libraries(${TEST_NAME} PRIVATE SDL3::SDL3)
    target_include_directories(${TEST_NAME} PRIVATE SDL/include/SDL3)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
