
# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)


# add_definitions(-DMC_USE_CUSTOM_CONF)
# add_definitions(-DNO_BOOT_PLAY_SHIT)

include_directories(./main/apps/utils/usb_keyboard_wrap/hid_device)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(cardputer)
