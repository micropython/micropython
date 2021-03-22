#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
// Pins 1-25
    { MP_ROM_QSTR(MP_QSTR_PE02), MP_ROM_PTR(&pin_PE02) },
    { MP_ROM_QSTR(MP_QSTR_PE03), MP_ROM_PTR(&pin_PE03) },
    { MP_ROM_QSTR(MP_QSTR_PE04), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_PE05), MP_ROM_PTR(&pin_PE05) },
    { MP_ROM_QSTR(MP_QSTR_PE06), MP_ROM_PTR(&pin_PE06) },
    /* VBAT -------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PC13), MP_ROM_PTR(&pin_PC13) },
    // PC14 OSC32_IN ----------------------------------*/
    // PC15 OSC32_OUT ---------------------------------*/
    // VSS --------------------------------------------*/
    // VDD --------------------------------------------*/
    // PH0 OSC_IN -------------------------------------*/
    // PH1 OSC_OUT ------------------------------------*/
    // NRST -------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PC00), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_PC01), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_PC02), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_PC03), MP_ROM_PTR(&pin_PC03) },
    // VSSA -------------------------------------------*/
    // VREF+ ------------------------------------------*/
    // VDDA -------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PA00), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_PA01), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_PA02), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_PA03), MP_ROM_PTR(&pin_PA03) },

// Pins 26-50
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
    { MP_ROM_QSTR(MP_QSTR_PE07), MP_ROM_PTR(&pin_PE07) },
    { MP_ROM_QSTR(MP_QSTR_PE08), MP_ROM_PTR(&pin_PE08) },
    { MP_ROM_QSTR(MP_QSTR_PE09), MP_ROM_PTR(&pin_PE09) },
    { MP_ROM_QSTR(MP_QSTR_PE10), MP_ROM_PTR(&pin_PE10) },
    { MP_ROM_QSTR(MP_QSTR_PE11), MP_ROM_PTR(&pin_PE11) },
    { MP_ROM_QSTR(MP_QSTR_PE12), MP_ROM_PTR(&pin_PE12) },
    { MP_ROM_QSTR(MP_QSTR_PE13), MP_ROM_PTR(&pin_PE13) },
    { MP_ROM_QSTR(MP_QSTR_PE14), MP_ROM_PTR(&pin_PE14) },
    { MP_ROM_QSTR(MP_QSTR_PE15), MP_ROM_PTR(&pin_PE15) },
    { MP_ROM_QSTR(MP_QSTR_PB10), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_PB11), MP_ROM_PTR(&pin_PB11) },
    // VCAP1 ------------------------------------------*/
    // VSS --------------------------------------------*/
    // VDD --------------------------------------------*/

// Pins 51-75
    { MP_ROM_QSTR(MP_QSTR_PB12), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_PB13), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_PB14), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_PB15), MP_ROM_PTR(&pin_PB15) },
    { MP_ROM_QSTR(MP_QSTR_PD08), MP_ROM_PTR(&pin_PD08) },
    { MP_ROM_QSTR(MP_QSTR_PD09), MP_ROM_PTR(&pin_PD09) },
    { MP_ROM_QSTR(MP_QSTR_PD10), MP_ROM_PTR(&pin_PD10) },
    { MP_ROM_QSTR(MP_QSTR_PD11), MP_ROM_PTR(&pin_PD11) },
    { MP_ROM_QSTR(MP_QSTR_PD12), MP_ROM_PTR(&pin_PD12) },
    { MP_ROM_QSTR(MP_QSTR_PD13), MP_ROM_PTR(&pin_PD13) },
    { MP_ROM_QSTR(MP_QSTR_PD14), MP_ROM_PTR(&pin_PD14) },
    { MP_ROM_QSTR(MP_QSTR_PD15), MP_ROM_PTR(&pin_PD15) },
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
    // VCAP2 ------------------------------------------*/
    // VSS --------------------------------------------*/
    // VDD --------------------------------------------*/

// Pins 76-100
    { MP_ROM_QSTR(MP_QSTR_PA14), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_PA15), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_PC10), MP_ROM_PTR(&pin_PC10) },
    { MP_ROM_QSTR(MP_QSTR_PC11), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_PC12), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_PD00), MP_ROM_PTR(&pin_PD00) },
    { MP_ROM_QSTR(MP_QSTR_PD01), MP_ROM_PTR(&pin_PD01) },
    { MP_ROM_QSTR(MP_QSTR_PD02), MP_ROM_PTR(&pin_PD02) },
    { MP_ROM_QSTR(MP_QSTR_PD03), MP_ROM_PTR(&pin_PD03) },
    { MP_ROM_QSTR(MP_QSTR_PD04), MP_ROM_PTR(&pin_PD04) },
    { MP_ROM_QSTR(MP_QSTR_PD05), MP_ROM_PTR(&pin_PD05) },
    { MP_ROM_QSTR(MP_QSTR_PD06), MP_ROM_PTR(&pin_PD06) },
    { MP_ROM_QSTR(MP_QSTR_PD07), MP_ROM_PTR(&pin_PD07) },
    { MP_ROM_QSTR(MP_QSTR_PB03), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_PB04), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_PB05), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_PB06), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_PB07), MP_ROM_PTR(&pin_PB07) },
    // BOOT0 ------------------------------------------*/
    { MP_ROM_QSTR(MP_QSTR_PB08), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_PB09), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_PE00), MP_ROM_PTR(&pin_PE00) },
    { MP_ROM_QSTR(MP_QSTR_PE01), MP_ROM_PTR(&pin_PE01) },
    // VSS --------------------------------------------*/
    // VDD --------------------------------------------*/

};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
