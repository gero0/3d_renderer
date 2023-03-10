cmake_minimum_required(VERSION 3.21.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(raster)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory(dependencies/minifb)
add_executable(raster "main.c")
target_sources(raster PRIVATE "algebra.c")
target_link_libraries(raster LINK_PUBLIC minifb)
target_include_directories(raster PRIVATE dependencies/minifb/include)
target_compile_options(raster PRIVATE -Wall -Wpedantic)
target_compile_options(raster PRIVATE -g)
