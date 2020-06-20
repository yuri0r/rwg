cmake_minimum_required(VERSION 3.16)
project(rwg C)

set(CMAKE_C_STANDARD 11)

add_executable(rwg rwg.c)

target_link_libraries(rwg cairo X11)
