include(boards/mpconfigboard_esp32c6_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_qio_80m
    boards/WAVESHARE_C6_TOUCH_LCD_1_47/sdkconfig.board
)
