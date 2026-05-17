set(IDF_TARGET esp32c5)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.riscv
    boards/sdkconfig.ble
    boards/sdkconfig.240mhz
    boards/sdkconfig.free_ram
    boards/sdkconfig.spiram
    boards/SPARKFUN_THINGPLUS_ESP32C5/sdkconfig.board
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
