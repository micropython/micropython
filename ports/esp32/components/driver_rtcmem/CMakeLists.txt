set(srcs
    "driver_rtcmem.c"
    "modrtcmem.c"
)

set(includes
    "include"
    )

# Compute the MicroPython root relative to this component
set(MP_ROOT ${PROJECT_DIR}/../..)

set(IDF_COMPONENTS
    nvs_flash
	esp_driver_gpio
	esp_driver_spi
	esp_mm
    micropython
)

idf_component_register(
	SRCS "${srcs}"
	INCLUDE_DIRS ${includes}
    PRIV_INCLUDE_DIRS
        ${MP_ROOT}             # micropython/
        ${MP_ROOT}/py          # micropython/py
        ${PROJECT_DIR}         # micropython/ports/esp32
        ${MICROPY_BOARD_DIR}   # board headers (mpconfigboard.h)
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${BUILD_DIR}
	REQUIRES ${IDF_COMPONENTS}
)
