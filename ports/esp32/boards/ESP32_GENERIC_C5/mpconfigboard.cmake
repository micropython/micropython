set(IDF_TARGET esp32c5)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.riscv
    boards/sdkconfig.ble
    boards/sdkconfig.240mhz
    boards/sdkconfig.free_ram
    boards/ESP32_GENERIC_C5/sdkconfig.board
)
