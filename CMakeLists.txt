cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME RTCM)
set(CMAKE_GENERATOR "MinGW Makefiles")

project(${PROJECT_NAME} C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIR})
link_directories(${GTK3_LIBRARY_DIR})

# add_executable(${PROJECT_NAME} WIN32 ../src/main.c ../res/res.rc)
add_executable(${PROJECT_NAME} ../src/main.c)
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE ${GTK3_CFLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${GTK3_LDFLAGS})
