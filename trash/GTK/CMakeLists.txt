# Cmake for project with GTK3 and OpenMP
cmake_minimum_required(VERSION 3.30)

project(RTCMwithGUI LANGUAGE C CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(SOURCE main.c)
add_executable(RTCM ${SOURCE})

target_include_directories("./include/")

