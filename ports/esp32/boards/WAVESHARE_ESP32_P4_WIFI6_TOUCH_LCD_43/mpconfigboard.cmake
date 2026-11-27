include(boards/mpconfigboard_esp32p4_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.p4_wifi_common
    boards/sdkconfig.p4_wifi_c6
    ${MICROPY_BOARD_DIR}/sdkconfig.board
)

if(NOT MICROPY_BOARD_VARIANT)
    list(APPEND SDKCONFIG_DEFAULTS ${MICROPY_BOARD_DIR}/sdkconfig.rev1)
endif()
