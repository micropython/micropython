#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "board.h"

// extended pins
STATIC const mp_rom_map_elem_t board_module_carrier_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PD09) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PD08) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PF15) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PE13) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PE03) },

    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PD01) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_CS), MP_ROM_PTR(&pin_PD00) },

    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_PTR(&pin_PG12) },
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_RX0), MP_ROM_PTR(&pin_PG08) },
    { MP_ROM_QSTR(MP_QSTR_TX0), MP_ROM_PTR(&pin_PG07) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PC03) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_PE01) },

    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_SCL3), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_SDA3), MP_ROM_PTR(&pin_PC09) },

    { MP_ROM_QSTR(MP_QSTR_RX2), MP_ROM_PTR(&pin_PD06) },
    { MP_ROM_QSTR(MP_QSTR_RTS2), MP_ROM_PTR(&pin_PD04) },
    { MP_ROM_QSTR(MP_QSTR_CTS2), MP_ROM_PTR(&pin_PD03) },
    { MP_ROM_QSTR(MP_QSTR_TX3), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_RX3), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_RTS3), MP_ROM_PTR(&pin_PD02) },
    { MP_ROM_QSTR(MP_QSTR_CTS3), MP_ROM_PTR(&pin_PB13) },

    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PE09) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PE11) },
    { MP_ROM_QSTR(MP_QSTR_SCL2), MP_ROM_PTR(&pin_PF01) },
    { MP_ROM_QSTR(MP_QSTR_SDA2), MP_ROM_PTR(&pin_PF00) },

    { MP_ROM_QSTR(MP_QSTR_QEN), MP_ROM_PTR(&pin_PD05) },
    { MP_ROM_QSTR(MP_QSTR_QCS), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_QCLK), MP_ROM_PTR(&pin_PE10) },

    { MP_ROM_QSTR(MP_QSTR_EN), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_TX2), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PD15) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PF12) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PF13) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_QIO3), MP_ROM_PTR(&pin_PE15) },
    { MP_ROM_QSTR(MP_QSTR_QIO2), MP_ROM_PTR(&pin_PE14) },
    { MP_ROM_QSTR(MP_QSTR_QIO1), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_QIO0), MP_ROM_PTR(&pin_PE12) },

};

MP_DEFINE_CONST_DICT(board_module_carrier, board_module_carrier_table);

MP_DEFINE_CONST_OBJ_TYPE(
    carrier_type,
    MP_QSTR_Ext,
    MP_TYPE_FLAG_NONE,
    locals_dict, &board_module_carrier
    );


// Core Feather Pins
STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_ext), MP_ROM_PTR(&carrier_type) },

    { MP_ROM_QSTR(MP_QSTR_ENABLE_3V3), &power_pin },
    { MP_ROM_QSTR(MP_QSTR_DISCHARGE_3V3), &discharge_pin },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA03) },     // PWM, ADC
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },     // PWM, ADC
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PC03) },     // ADC
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PC01) },     // ADC
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PC04) },     // ADC
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PC05) },     // ADC

    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_USR), MP_ROM_PTR(&pin_PC13) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_PB02) },

    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PE11) },     // PWM
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PE09) },     // PWM
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PD15) },     // PWM
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA04) },    // ADC, DAC1 output also
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA07) },    // ADC, PWM
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA06) },    // ADC, PWM

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PE02) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA05) },   // ADC, PWM, DAC2 output also

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB07) },    // PWM
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB06) },    // PWM

    { MP_ROM_QSTR(MP_QSTR_DAC1), MP_ROM_PTR(&pin_PA04) },   // D10
    { MP_ROM_QSTR(MP_QSTR_DAC2), MP_ROM_PTR(&pin_PA05) },   // D13

    { MP_ROM_QSTR(MP_QSTR_SS), MP_ROM_PTR(&pin_PD00) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PD01) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB14) },   // PWM?
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB15) },   // PWM?

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA09) },     // ADC, PWM
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA10) },     // PWM

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_CLOCK), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_MICROPHONE_DATA), MP_ROM_PTR(&pin_PC03) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
