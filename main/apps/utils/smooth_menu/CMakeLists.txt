# Basic CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# SMOOTH_MENU
file(GLOB_RECURSE SMOOTH_MENU_SRCS
    src/*.cpp
)
set(SMOOTH_MENU_INCS
    src/
    # src/lv_anim/
    # src/menu/
    # src/selector/
    # src/camera/
    # src/simple_menu/
)



#  Build library
add_library(smooth_menu
    ${SMOOTH_MENU_SRCS}
)


# Include
target_include_directories(smooth_menu PUBLIC
    ${SMOOTH_MENU_INCS}
)

