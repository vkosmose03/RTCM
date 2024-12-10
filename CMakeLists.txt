cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME RTCM)
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} WIN32 ./src/main.c)
add_library(RTCMpars ./src/func.c)

