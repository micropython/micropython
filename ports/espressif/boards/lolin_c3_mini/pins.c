#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // C3 Mini Board
    // From Wemos C3 Mini Schematic
    // https://www.wemos.cc/en/latest/_static/files/sch_c3_mini_v1.0.0.pdf
    // Starting on Left side going counterclockwise
    // And MP Config
    // https://github.com/micropython/micropython/blob/master/ports/esp32/boards/LOLIN_C3_MINI

    // 1: LAN_IN
    // 2: VDD3P3
    // 3: VDD3P3
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },// Reset Button
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO0) },    // 4: GPIO0/32K_P/A0
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO1) },    // 5: GPIO1/32K_N/A1
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO2) },    // 6: GPIO2/A2/FSPIQ
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO2) },   // mpboardconfig.h
    // 7: CHIP_EN
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO3) },    // 8: GPIO3/A3
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO3) },  // mpboardconfig.h
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_GPIO4) },    // 9: GPIO4/A4/FSPIHD/MTMS
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO4) },  // mpboardconfig.h
    { MP_ROM_QSTR(MP_QSTR_IO5), MP_ROM_PTR(&pin_GPIO5) },   // 10: GPIO5/A2_0/FSPIWP/MTDI
    // 11: VDD3P3_RTC
    { MP_ROM_QSTR(MP_QSTR_IO6), MP_ROM_PTR(&pin_GPIO6) },   // 12: GPIO6/FSPICLK/MTCK
    { MP_ROM_QSTR(MP_QSTR_LED),  MP_ROM_PTR(&pin_GPIO7) },  // mpboardconfig.h
    { MP_ROM_QSTR(MP_QSTR_IO7), MP_ROM_PTR(&pin_GPIO7) },   // 13: GPIO7/FSPID/MTD0
    { MP_ROM_QSTR(MP_QSTR_IO8), MP_ROM_PTR(&pin_GPIO8) },   // 14: GPIO8
    { MP_ROM_QSTR(MP_QSTR_SDA),  MP_ROM_PTR(&pin_GPIO8) },  // mpboardconfig.h
    { MP_ROM_QSTR(MP_QSTR_IO9), MP_ROM_PTR(&pin_GPIO9) },   // 15: GPIO9
    { MP_ROM_QSTR(MP_QSTR_IO20), MP_ROM_PTR(&pin_GPIO20) }, // 16: GPIO10/FSPICS0
    { MP_ROM_QSTR(MP_QSTR_SCL),  MP_ROM_PTR(&pin_GPIO10) }, // mpboardconfig.h
    // 17: VDD3P3_SPI
    // 18: VDD_SPI
    // 19: SPIHD
    // 20: SPIWP
    // 21: SPICSO
    // 22: SPICLK
    // 23: SPID
    // 24: SPIQ
    // Used for USB 25: GPIO18/D-
    // Used for USB 26: GPIO19/D+
    { MP_ROM_QSTR(MP_QSTR_RX),   MP_ROM_PTR(&pin_GPIO20) }, // 27: GPIO20/U0RXD
    { MP_ROM_QSTR(MP_QSTR_RX),   MP_ROM_PTR(&pin_GPIO22) }, // 28: FPIO22/U0TXD
    // 29: XTAL_N
    // 30: XTAL_P
    // 31: VDDA
    // 32: VDDA
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },// board singleton implicit from schematic/shield standard
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
