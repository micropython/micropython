#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_obj_tuple_t lcd_data_tuple = {
    {&mp_type_tuple},
    8,
    {
        MP_ROM_PTR(&pin_GPIO1),
        MP_ROM_PTR(&pin_GPIO10),
        MP_ROM_PTR(&pin_GPIO2),
        MP_ROM_PTR(&pin_GPIO11),
        MP_ROM_PTR(&pin_GPIO3),
        MP_ROM_PTR(&pin_GPIO12),
        MP_ROM_PTR(&pin_GPIO4),
        MP_ROM_PTR(&pin_GPIO13),
        MP_ROM_PTR(&pin_GPIO5),
        MP_ROM_PTR(&pin_GPIO14),
        MP_ROM_PTR(&pin_GPIO6),
        MP_ROM_PTR(&pin_GPIO15),
        MP_ROM_PTR(&pin_GPIO7),
        MP_ROM_PTR(&pin_GPIO16),
        MP_ROM_PTR(&pin_GPIO8),
        MP_ROM_PTR(&pin_GPIO17),
    }
};

STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RS), MP_ROM_PTR(&pin_GPIO38) },

    // LCD
    { MP_ROM_QSTR(MP_QSTR_LCD_DATA), MP_ROM_PTR(&lcd_data_tuple) },
    { MP_ROM_QSTR(MP_QSTR_LCD_WR), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RS), MP_ROM_PTR(&pin_GPIO38) },

    // SPI and SD
    { MP_ROM_QSTR(MP_QSTR_CS_SD), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_CS_CNN), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(DEFAULT_SPI_BUS_MISO) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(DEFAULT_SPI_BUS_MOSI) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(DEFAULT_SPI_BUS_SCK) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },

    // I2C
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(DEFAULT_I2C_BUS_SCL) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(DEFAULT_I2C_BUS_SDA) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // canbus (TWAI)
    { MP_ROM_QSTR(MP_QSTR_CAN_RX),  MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_CAN_TX),  MP_ROM_PTR(&pin_GPIO42) },

    // Audio
    { MP_ROM_QSTR(MP_QSTR_MIC_ADC_M),  MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_DAC_OUT),  MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_SPI_MISO), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_SPI_MOSI), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_SPI_SCK), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_SPI_CS), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_BT_ADC), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_SCL), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_SDA), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S0_MCLK), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S0_BCLK), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S0_LRCK), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S0_SDI), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S0_SDO), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_RST), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_WAKE_INT), MP_ROM_PTR(&pin_GPIO46) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S1_MCLK), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_PA_CTRL), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S1_SDI), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S1_SDO), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S1_LRCK_DAC1), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S1_BCLK_DAC2), MP_ROM_PTR(&pin_GPIO18) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
