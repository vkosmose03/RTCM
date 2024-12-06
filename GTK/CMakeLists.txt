cmake_minimum_required(VERSION 3.30)
project(GTKTutorial)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set PKG_CONFIG_PATH for GTK3
set(ENV{PKG_CONFIG_PATH} "D:/MSYS2/ucrt64/lib/pkgconfig")
set(GLIB_NO_DBUS 1)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# Find GTK3 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Добавляем ресурсный файл (иконка)
set(SOURCE_FILES main.c resource.rc)

# Include GTK3 headers and libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Define the executable
add_executable(TutorialApp main.c)

# Компилируем ресурс с помощью windres
if(WIN32)
  target_sources(TutorialApp PRIVATE res.o)
endif()

# Link GTK3 libraries to the executable
target_link_libraries(TutorialApp ${GTK3_LIBRARIES})