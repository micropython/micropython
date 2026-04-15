if(CONFIG_DRIVER_BADGE_BSP_ENABLE)
    set(srcs
        "driver_badge_bsp.c"
        "modbadgebsp.c"
    )
    set(includes
    "include"
    )
else()
    set(srcs "")
    set(includes
    ""
    )
endif()

# Compute the MicroPython root relative to this component
set(MP_ROOT ${PROJECT_DIR}/../..)

idf_component_register(
	SRCS "${srcs}"
    PRIV_INCLUDE_DIRS
        ${MP_ROOT}             # micropython/
        ${MP_ROOT}/py          # micropython/py
        ${PROJECT_DIR}         # micropython/ports/esp32
        ${MICROPY_BOARD_DIR}   # board headers (mpconfigboard.h)
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${BUILD_DIR}
	INCLUDE_DIRS ${includes}
	REQUIRES micropython badge-bsp esp_mm
)
