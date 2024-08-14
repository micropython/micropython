# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(ENV{IDF_PATH} ${CMAKE_SOURCE_DIR}/esp-idf)

# The component list here determines what options we get in menuconfig and what the ninja file can build.
set(COMPONENTS bt driver esp_driver_dac esp_driver_gpio esp_driver_gptimer esp_driver_i2s esp_driver_ledc esp_driver_pcnt esp_driver_rmt esp_driver_spi esp_driver_tsens esp_driver_uart esp-tls esp_adc_cal esp_event esp_netif esp_psram esp_wifi esptool_py freertos log lwip main mbedtls mdns soc ulp usb wpa_supplicant esp-camera esp_lcd vfs esp_vfs_console)
set(EXTRA_COMPONENT_DIRS "esp-protocols/components/mdns" "esp-camera")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(circuitpython)

idf_build_set_property(__OUTPUT_SDKCONFIG 0)
