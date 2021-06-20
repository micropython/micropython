#include "shared-bindings/board/__init__.h"

#include "supervisor/board.h"

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    // Analog
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO_AD_14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO_AD_11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO_AD_10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO_AD_09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_GPIO_AD_08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_GPIO_AD_07) },

    // Digital
    { MP_OBJ_NEW_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO_06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_GPIO_05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_GPIO_04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_GPIO_03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_GPIO_02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_GPIO_01) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO_00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_GPIO_00) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_GPIO_AD_05) },

    // SPI
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO_AD_06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO_AD_03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO_AD_04) },

    // UART
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO_AD_02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO_AD_01) },

    // I2C
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO_09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO_10) },

    // ESP control
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_CS), MP_ROM_PTR(&pin_GPIO_13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_GPIO0), MP_ROM_PTR(&pin_GPIO_07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_BUSY), MP_ROM_PTR(&pin_GPIO_08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RESET), MP_ROM_PTR(&pin_GPIO_AD_00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_TX), MP_ROM_PTR(&pin_GPIO_12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RX), MP_ROM_PTR(&pin_GPIO_11) },
    // { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RTS), MP_ROM_PTR(&pin_) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO_SD_05) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
