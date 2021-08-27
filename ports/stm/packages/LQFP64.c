#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
// Pins 1-16
    /* VBAT -------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PC13), MP_ROM_PTR(&pin_PC13) },
    // PC14 OSC32_IN ----------------------------------*/
    // PC15 OSC32_OUT ---------------------------------*/
    // PH0 OSC_IN -------------------------------------*/
    // PH1 OSC_OUT ------------------------------------*/
    // NRST -------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PC00), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_PC01), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_PC02), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_PC03), MP_ROM_PTR(&pin_PC03) },
    // VSSA -------------------------------------------*/
    // VDDA -------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PA00), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_PA01), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_PA02), MP_ROM_PTR(&pin_PA02) },

// Pins 17-32
    { MP_ROM_QSTR(MP_QSTR_PA03), MP_ROM_PTR(&pin_PA03) },
    // VSS --------------------------------------------*/
    // VDD --------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PA04), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_PA05), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_PA06), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_PA07), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_PC04), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_PC05), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_PB00), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_PB01), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_PB02), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_PB10), MP_ROM_PTR(&pin_PB10) },
    #if defined(STM32F405xx) || defined(STM32F412Zx) || defined(STM32F407xx) || defined(STM32F767xx)
    { MP_ROM_QSTR(MP_QSTR_PB11), MP_ROM_PTR(&pin_PB11) },
    #endif // or VCAP1 -----------------------------------*/
    // VCAP1 or VSS -----------------------------------*/
    // VDD --------------------------------------------*/

// Pins 33-48
    { MP_ROM_QSTR(MP_QSTR_PB12), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_PB13), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_PB14), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_PB15), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_PC06), MP_ROM_PTR(&pin_PC06) },
    { MP_ROM_QSTR(MP_QSTR_PC07), MP_ROM_PTR(&pin_PC07) },
    { MP_ROM_QSTR(MP_QSTR_PC08), MP_ROM_PTR(&pin_PC08) },
    { MP_ROM_QSTR(MP_QSTR_PC09), MP_ROM_PTR(&pin_PC09) },
    { MP_ROM_QSTR(MP_QSTR_PA08), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_PA09), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_PA10), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_PA11), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_PA12), MP_ROM_PTR(&pin_PA12) },
    { MP_ROM_QSTR(MP_QSTR_PA13), MP_ROM_PTR(&pin_PA13) },
    // VSS or VCAP (F405) -----------------------------*/
    // VDD --------------------------------------------*/

// Pins 49-64
    { MP_ROM_QSTR(MP_QSTR_PA14), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_PA15), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_PC10), MP_ROM_PTR(&pin_PC10) },
    { MP_ROM_QSTR(MP_QSTR_PC11), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_PC12), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_PD02), MP_ROM_PTR(&pin_PD02) },
    { MP_ROM_QSTR(MP_QSTR_PB03), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_PB04), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_PB05), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_PB06), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_PB07), MP_ROM_PTR(&pin_PB07) },
    // BOOT0 ------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PB08), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_PB09), MP_ROM_PTR(&pin_PB09) },
    // VSS --------------------------------------------*/
    // VDD --------------------------------------------*/

};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
