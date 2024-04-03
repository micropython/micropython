#include "shared-bindings/board/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(touch_i2c, i2c, 1)

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_IO0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_USER_BUTTON), MP_ROM_PTR(&pin_GPIO0) },

    // Buzzer
    { MP_ROM_QSTR(MP_QSTR_BUZZER), MP_ROM_PTR(&pin_GPIO18) },

    // SPI
    { MP_ROM_QSTR(MP_QSTR_MO), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_MI), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_CLK), MP_ROM_PTR(&pin_GPIO36) },

    // TFT Screen IO
    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO13) },

    // I2C - BUS1 - Main Peripherals
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO9) },

    // I2C - BUS2 - Touch IC
    { MP_ROM_QSTR(MP_QSTR_TOUCH_I2C), MP_ROM_PTR(&board_touch_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDA2), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_SCL2), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_INT), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH_RESET), MP_ROM_PTR(&pin_GPIO12) },

    // RTC Interrupt Pin
    { MP_ROM_QSTR(MP_QSTR_RTC_INT), MP_ROM_PTR(&pin_GPIO4) },

    // Fuel Gauge Interrupt/Alert Pin
    { MP_ROM_QSTR(MP_QSTR_FG_ALERT), MP_ROM_PTR(&pin_GPIO14) },

    // BMI270 Interrupt Pins
    { MP_ROM_QSTR(MP_QSTR_BMI_INT1), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_BMI_INT2), MP_ROM_PTR(&pin_GPIO7) },

    // I2S Mic ICS-43434
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_DATA), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_BCLK), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MIC_WS), MP_ROM_PTR(&pin_GPIO3) },

    // User Power Control
    { MP_ROM_QSTR(MP_QSTR_USER_PWR_SHUTDOWN), MP_ROM_PTR(&pin_GPIO21) },

    // External user FLASH CS pin
    { MP_ROM_QSTR(MP_QSTR_EXT_FLASH_CS), MP_ROM_PTR(&pin_GPIO33) },

    // VBUS sense IO to detect if USB power is present
    { MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_GPIO34) },

    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
