set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
    boards/sdkconfig.240mhz
    boards/sdkconfig.spiram
    boards/M5STACK_CORE2/sdkconfig.board
)

set(LV_CFLAGS -DLV_COLOR_DEPTH=16 -DLV_COLOR_16_SWAP=1)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(USER_C_MODULES ${MICROPY_BOARD_DIR}/../../../../user_modules/lv_binding_micropython/micropython.cmake)