set(IDF_TARGET esp32c61)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.riscv
    boards/sdkconfig.ble
    boards/sdkconfig.flash_qio_80m
    boards/sdkconfig.spiram_quad
    ${MICROPY_BOARD_DIR}/sdkconfig.board
)
