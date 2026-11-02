# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(ENV{IDF_PATH} ${CMAKE_SOURCE_DIR}/esp-idf)

# The component list here determines what options we get in menuconfig and what the ninja file can build.
# Keep this split into separate set()/list() calls so additions merge easily.
# Core components
set(COMPONENTS bt driver esptool_py freertos log main mbedtls pthread soc)
# Peripheral drivers
list(APPEND COMPONENTS esp_driver_dac esp_driver_gpio esp_driver_gptimer esp_driver_i2c esp_driver_i2s esp_driver_jpeg esp_driver_ledc esp_driver_pcnt esp_driver_rmt esp_driver_sdmmc esp_driver_spi esp_driver_touch_sens esp_driver_tsens esp_driver_uart)
# Networking
list(APPEND COMPONENTS esp_eth esp-tls esp_event esp_netif esp_wifi lwip mdns wpa_supplicant)
# Misc
list(APPEND COMPONENTS esp_adc esp_adc_cal esp_psram esp_security ulp vfs esp_vfs_console esp_stdio sdmmc usb)
# Display and camera
list(APPEND COMPONENTS esp-camera esp_lcd)

if(CIRCUITPY_MIPICSI)
    list(APPEND COMPONENTS esp_driver_cam esp_hal_cam)
endif()

set(EXTRA_COMPONENT_DIRS "esp-protocols/components/mdns" "esp-camera")

if(CIRCUITPY_ESP_HOSTED)
    list(APPEND EXTRA_COMPONENT_DIRS "esp-hosted")
    list(APPEND COMPONENTS esp-hosted)
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(ENV{ESP_IDF_VERSION} "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")
project(circuitpython)

idf_build_set_property(__OUTPUT_SDKCONFIG 0)
