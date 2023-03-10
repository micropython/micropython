cmake_minimum_required(VERSION 3.12)
project(pioasm)
set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/tinyusb)
include(../sdk/pico_sdk_init.cmake)
pico_sdk_init()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PICO_SDK_PATH}/tools)
find_package(Pioasm REQUIRED)
