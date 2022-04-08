#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"

// Core Feather Pins
STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_ENABLE_3V3), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_DISCHARGE_3V3), MP_ROM_PTR(&pin_PE06) },
    { MP_ROM_QSTR(MP_QSTR_DISABLE_DISCHARGING), MP_ROM_TRUE },
    { MP_ROM_QSTR(MP_QSTR_ENABLE_DISCHARGING), MP_ROM_FALSE },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PC03) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_USR), MP_ROM_PTR(&pin_PC13) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_PB02) }, // boot button, but looks like it's wired to GND on the schematic

    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PE11) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PE09) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PD15) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA04) },    // DAC1 output also
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA06) },

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PE02) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA05) },   // DAC1 output also

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB06) },

    { MP_ROM_QSTR(MP_QSTR_DAC1), MP_ROM_PTR(&pin_PA04) },   // D10
    { MP_ROM_QSTR(MP_QSTR_DAC2), MP_ROM_PTR(&pin_PA05) },   // D13

    { MP_ROM_QSTR(MP_QSTR_SS), MP_ROM_PTR(&pin_PD00) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PD01) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PB15) },

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA10) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
