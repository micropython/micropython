#message(STATUS "CONFIG_DRIVER_MCH22_RP2040_ENABLE: ${CONFIG_DRIVER_MCH22_RP2040_ENABLE}")
if(CONFIG_DRIVER_MCH22_RP2040_ENABLE)
    set(srcs
        "esp32-component-mch2022-rp2040/rp2040.c"
        "esp32-component-spi-ice40/ice40.c"
        "modmch22.c"
    )
else()
    set(srcs "")
endif()

set(include
    "include"
    "esp32-component-mch2022-rp2040/include"
    "esp32-component-spi-ice40/include"
    )

#   REQUIRES micropython buses driver_display_ili9341

# Compute the MicroPython root relative to this component
set(MP_ROOT ${PROJECT_DIR}/../..)

#message(STATUS ${MP_GENHDR})
idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "${include}"
    PRIV_INCLUDE_DIRS
        ${MP_ROOT}             # micropython/
        ${MP_ROOT}/py          # micropython/py
        ${PROJECT_DIR}         # micropython/ports/esp32
        ${MICROPY_BOARD_DIR}   # board headers (mpconfigboard.h)
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${BUILD_DIR}
    REQUIRES
        micropython
		buses
		driver_display_ili9341
        driver
        freertos
        esp_timer
        esp_hw_support
        esp_mm
)
