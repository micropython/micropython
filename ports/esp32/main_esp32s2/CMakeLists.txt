# Set location of base MicroPython directory.
if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../.. ABSOLUTE)
endif()

# Set location of the ESP32 port directory.
if(NOT MICROPY_PORT_DIR)
    get_filename_component(MICROPY_PORT_DIR ${MICROPY_DIR}/ports/esp32 ABSOLUTE)
endif()

set(MICROPY_PY_TINYUSB ON)

if(PREFER_FSM_ULP)
list(APPEND MICROPY_DEF_CORE
    CONFIG_IDF_TARGET_ULP_PREFER_FSM=1
)
endif()

include(${MICROPY_PORT_DIR}/esp32_common.cmake)
