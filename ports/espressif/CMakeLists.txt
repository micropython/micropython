# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.13)

set(ENV{IDF_PATH} ${CMAKE_SOURCE_DIR}/esp-idf)

# The component list here determines what options we get in menuconfig and what the ninja file
# can build.
set(COMPONENTS esptool_py soc driver log main esp-tls mbedtls esp_event esp_adc_cal esp_netif esp_wifi lwip wpa_supplicant freertos)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(circuitpython)

idf_build_set_property(__OUTPUT_SDKCONFIG 0)
