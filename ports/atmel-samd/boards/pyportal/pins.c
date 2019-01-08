#include "shared-bindings/board/__init__.h"

#include "board_busses.h"

// This mapping only includes functional names because pins broken
// out on connectors are labeled with their MCU name available from
// microcontroller.pin.
STATIC const mp_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_AUDIO_OUT),  (mp_obj_t)&pin_PA02 },

    { MP_OBJ_NEW_QSTR(MP_QSTR_D13),  (mp_obj_t)&pin_PA27 },

    // LCD pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_RESET),  (mp_obj_t)&pin_PA00 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_RD),  (mp_obj_t)&pin_PB04 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_RS),  (mp_obj_t)&pin_PB05 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_CS),  (mp_obj_t)&pin_PB06 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_TE),  (mp_obj_t)&pin_PB07 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_WR),  (mp_obj_t)&pin_PB09 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT_BACKLIGHT),  (mp_obj_t)&pin_PB31 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA0),  (mp_obj_t)&pin_PA16 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA1),  (mp_obj_t)&pin_PA17 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA2),  (mp_obj_t)&pin_PA18 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA3),  (mp_obj_t)&pin_PA19 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA4),  (mp_obj_t)&pin_PA20 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA5),  (mp_obj_t)&pin_PA21 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA6),  (mp_obj_t)&pin_PA22 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD_DATA7),  (mp_obj_t)&pin_PA23 },

    // Touch pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_TOUCH_YD),  (mp_obj_t)&pin_PA04 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TOUCH_XL),  (mp_obj_t)&pin_PA05 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TOUCH_YU),  (mp_obj_t)&pin_PA06 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TOUCH_XR),  (mp_obj_t)&pin_PB08 },

    // ESP control
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_CS),  (mp_obj_t)&pin_PA15 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_BUSY),  (mp_obj_t)&pin_PB14 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_GPIO0),  (mp_obj_t)&pin_PB15 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RESET),  (mp_obj_t)&pin_PB16 },


    // SD Card
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MOSI), (mp_obj_t)&pin_PA12 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_SCK), (mp_obj_t)&pin_PA13 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CS), (mp_obj_t)&pin_PB30 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_MISO), (mp_obj_t)&pin_PA14 },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CARD_DETECT), (mp_obj_t)&pin_PA01 },

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), (mp_obj_t)&pin_PB22 },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
