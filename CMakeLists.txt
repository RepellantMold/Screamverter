cmake_minimum_required(VERSION 3.0)
project(Screamverter VERSION 0.1)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS FALSE)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

add_executable(Screamverter main.c mainheader.c pattern.c sample.c)