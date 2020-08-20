#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {

    /*Port and bus pins*/
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_P0_14) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_P0_13) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_P0_15) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P0_12) },

    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    /*TG-Watch02A specific pins*/

        //tft / display pins
        { MP_ROM_QSTR(MP_QSTR_BACKLIGHT), MP_ROM_PTR(&pin_P0_07) },
        { MP_ROM_QSTR(MP_QSTR_DISP_PWR), MP_ROM_PTR(&pin_P0_05) },
        { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_P1_14) },
        { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_P1_12) },
        { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_PTR(&pin_P1_01) },

        //cap touch screen
        { MP_ROM_QSTR(MP_QSTR_CTP_RESET), MP_ROM_PTR(&pin_P1_03) },
        { MP_ROM_QSTR(MP_QSTR_CTP_INT), MP_ROM_PTR(&pin_P1_05) },

        //power / battery
        { MP_ROM_QSTR(MP_QSTR_CHRG_STAT), MP_ROM_PTR(&pin_P0_06) },
        { MP_ROM_QSTR(MP_QSTR_BAT_VDIV), MP_ROM_PTR(&pin_P0_29) },
        { MP_ROM_QSTR(MP_QSTR_VBUS_PRESENT), MP_ROM_PTR(&pin_P1_04) },

        //sensors / outputs
        { MP_ROM_QSTR(MP_QSTR_RTC_RESET), MP_ROM_PTR(&pin_P0_26) },
        { MP_ROM_QSTR(MP_QSTR_RTC_INT), MP_ROM_PTR(&pin_P0_27) },

        { MP_ROM_QSTR(MP_QSTR_ACCEL_INT1), MP_ROM_PTR(&pin_P1_11) },
        { MP_ROM_QSTR(MP_QSTR_ACCEL_INT2), MP_ROM_PTR(&pin_P1_10) },

        { MP_ROM_QSTR(MP_QSTR_HAPTIC_INT), MP_ROM_PTR(&pin_P1_07) },
        { MP_ROM_QSTR(MP_QSTR_HAPTIC_ENABLE), MP_ROM_PTR(&pin_P1_06) },

        //smc pins
        { MP_ROM_QSTR(MP_QSTR_SMC_RESET), MP_ROM_PTR(&pin_P0_08) },
        { MP_ROM_QSTR(MP_QSTR_BUTTON0), MP_ROM_PTR(&pin_P1_08) },
        { MP_ROM_QSTR(MP_QSTR_BUTTON1), MP_ROM_PTR(&pin_P1_09) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
