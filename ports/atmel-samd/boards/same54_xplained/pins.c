#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_obj_tuple_t sdio_data_tuple = {
    {&mp_type_tuple},
    4,
    {
        MP_ROM_PTR(&pin_PB18),
        MP_ROM_PTR(&pin_PB19),
        MP_ROM_PTR(&pin_PB20),
        MP_ROM_PTR(&pin_PB21),
    }
};


// This mapping only includes functional names because pins broken
// out on connectors are labeled with their MCU name available from
// microcontroller.pin.
STATIC const mp_rom_map_elem_t board_global_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA),  MP_ROM_PTR(&pin_PD08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL),  MP_ROM_PTR(&pin_PD09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VSYNC),  MP_ROM_PTR(&pin_PA12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HSYNC),  MP_ROM_PTR(&pin_PA13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PCLK),  MP_ROM_PTR(&pin_PA14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_XCLK),  MP_ROM_PTR(&pin_PA15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D07),  MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D06),  MP_ROM_PTR(&pin_PA22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D05),  MP_ROM_PTR(&pin_PA21) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D04),  MP_ROM_PTR(&pin_PA20) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D03),  MP_ROM_PTR(&pin_PA19) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D02),  MP_ROM_PTR(&pin_PA18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D01),  MP_ROM_PTR(&pin_PA17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D00),  MP_ROM_PTR(&pin_PA16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D09),  MP_ROM_PTR(&pin_PB15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D08),  MP_ROM_PTR(&pin_PB14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RST),  MP_ROM_PTR(&pin_PC12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PWDN),  MP_ROM_PTR(&pin_PC11) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_PDEC_A),  MP_ROM_PTR(&pin_PC16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PDEC_B),  MP_ROM_PTR(&pin_PC17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PDEC_C),  MP_ROM_PTR(&pin_PC18) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_ADCDAC),  MP_ROM_PTR(&pin_PA02) },


    { MP_OBJ_NEW_QSTR(MP_QSTR_SW0),  MP_ROM_PTR(&pin_PB31) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED),  MP_ROM_PTR(&pin_PC18) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_QT),  MP_ROM_PTR(&pin_PA16) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_CAN_RX),  MP_ROM_PTR(&pin_PB13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CAN_TX),  MP_ROM_PTR(&pin_PB12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CAN_STANDBY),  MP_ROM_PTR(&pin_PC13) },

    // EXT1 header
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB04),  MP_ROM_PTR(&pin_PB04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB05),  MP_ROM_PTR(&pin_PB05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA06),  MP_ROM_PTR(&pin_PA06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA07),  MP_ROM_PTR(&pin_PA07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB08),  MP_ROM_PTR(&pin_PB08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB09),  MP_ROM_PTR(&pin_PB09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB07),  MP_ROM_PTR(&pin_PB07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA27),  MP_ROM_PTR(&pin_PA27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC22),  MP_ROM_PTR(&pin_PC22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA23),  MP_ROM_PTR(&pin_PA23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA05),  MP_ROM_PTR(&pin_PA05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA04),  MP_ROM_PTR(&pin_PA04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB28),  MP_ROM_PTR(&pin_PB28) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB27),  MP_ROM_PTR(&pin_PB27) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB29),  MP_ROM_PTR(&pin_PB29) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB26),  MP_ROM_PTR(&pin_PB26) },

    // EXT2 header
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB00),  MP_ROM_PTR(&pin_PB00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA03),  MP_ROM_PTR(&pin_PA03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB01),  MP_ROM_PTR(&pin_PB01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB06),  MP_ROM_PTR(&pin_PB06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB14),  MP_ROM_PTR(&pin_PB14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB15),  MP_ROM_PTR(&pin_PB15) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD00),  MP_ROM_PTR(&pin_PD00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB02),  MP_ROM_PTR(&pin_PB02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD08),  MP_ROM_PTR(&pin_PD08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD09),  MP_ROM_PTR(&pin_PD09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB17),  MP_ROM_PTR(&pin_PB17) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB16),  MP_ROM_PTR(&pin_PB16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC06),  MP_ROM_PTR(&pin_PC06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC04),  MP_ROM_PTR(&pin_PC04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC07),  MP_ROM_PTR(&pin_PC07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC05),  MP_ROM_PTR(&pin_PC05) },

    // EXT3 header
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC02),  MP_ROM_PTR(&pin_PC02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC03),  MP_ROM_PTR(&pin_PC03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC10),  MP_ROM_PTR(&pin_PC10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC01),  MP_ROM_PTR(&pin_PC01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD11),  MP_ROM_PTR(&pin_PD11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD10),  MP_ROM_PTR(&pin_PD10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC31),  MP_ROM_PTR(&pin_PC31) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC30),  MP_ROM_PTR(&pin_PC30) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD09),  MP_ROM_PTR(&pin_PD09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD08),  MP_ROM_PTR(&pin_PD08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC22),  MP_ROM_PTR(&pin_PC22) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC23),  MP_ROM_PTR(&pin_PC23) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC04),  MP_ROM_PTR(&pin_PC04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC14),  MP_ROM_PTR(&pin_PC14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC05),  MP_ROM_PTR(&pin_PC05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC07),  MP_ROM_PTR(&pin_PC07) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_SDIO_CLOCK), MP_ROM_PTR(&pin_PA21) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_COMMAND), MP_ROM_PTR(&pin_PA20) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA), MP_ROM_PTR(&sdio_data_tuple) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
