cmake_minimum_required(VERSION 3.0)
project(Screamverter VERSION 0.1)

set(CMAKE_C_STANDARD 90)

add_executable(Screamverter main.c mainheader.c pattern.c sample.c)