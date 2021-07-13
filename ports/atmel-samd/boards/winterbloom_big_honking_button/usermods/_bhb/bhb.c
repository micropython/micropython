#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "samd/pins.h"
#include "sam.h"

STATIC mp_obj_t _bhb_read_adc(void);

STATIC mp_obj_t _bhb_init_adc(void) {
    claim_pin(&pin_PB08);
    common_hal_never_reset_pin(&pin_PB08);

    /* Enable the APB clock for the ADC. */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;

    /* Enable GCLK0 for the ADC */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN_GCLK0 |
        GCLK_CLKCTRL_ID_ADC;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    ;

    uint32_t bias = (*((uint32_t *)ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
    uint32_t linearity = (*((uint32_t *)ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    linearity |= ((*((uint32_t *)ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {
    }
    ;

    /* Write the calibration data. */
    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);

    /* Use the internal VCC reference. This is 1/2 of what's on VCCA.
       since VCCA is 3.3v, this is 1.65v.
    */
    ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INTVCC1;

    /* Capture 64 samples. */
    ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_64 | ADC_AVGCTRL_ADJRES(4);

    /* Set the clock prescaler to 32, which is the same as the CircuitPython default.
       Set the resolution to 16 for averaging
    */
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 |
        ADC_CTRLB_RESSEL_16BIT;

    /* Configure the input parameters.

    - GAIN_DIV2 means that the input voltage is halved. This is important
      because the voltage reference is 1/2 of VCCA. So if you want to
      measure 0-3.3v, you need to halve the input as well.

    - MUXNEG_GND means that the ADC should compare the input value to GND.

    - MUXPOS_PIN3 means that the ADC should read from AIN2, or PB08.
    */
    ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2 |
        ADC_INPUTCTRL_MUXNEG_GND |
        ADC_INPUTCTRL_MUXPOS_PIN2;


    /* Set PB08 as an input pin. */
    PORT->Group[1].DIRCLR.reg = PORT_PB08;

    /* Enable the peripheral multiplexer for PB08. */
    PORT->Group[1].PINCFG[8].reg |= PORT_PINCFG_PMUXEN;

    /* Set PB08 to function B which is analog input. */
    PORT->Group[1].PMUX[4].reg |= PORT_PMUX_PMUXE_B;

    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {
    }
    ;

    /* Enable the ADC. */
    ADC->CTRLA.bit.ENABLE = true;

    /* Make one read and throw it away, as per the datasheet. */
    _bhb_read_adc();

    return mp_const_none;
}

STATIC mp_obj_t _bhb_read_adc(void) {
    /* Wait for bus synchronization. */
    while (ADC->STATUS.bit.SYNCBUSY) {
    }
    ;

    /* Start the ADC using a software trigger. */
    ADC->SWTRIG.bit.START = true;

    /* Wait for the result ready flag to be set. */
    while (ADC->INTFLAG.bit.RESRDY == 0) {
        ;
    }

    /* Clear the flag. */
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;

    /* Read the value. */
    uint32_t result = ADC->RESULT.reg;

    return MP_OBJ_NEW_SMALL_INT(result);
}


STATIC MP_DEFINE_CONST_FUN_OBJ_0(_bhb_init_adc_obj, _bhb_init_adc);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(_bhb_read_adc_obj, _bhb_read_adc);

STATIC const mp_rom_map_elem_t _bhb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__bhb) },
    { MP_ROM_QSTR(MP_QSTR_init_adc), MP_ROM_PTR(&_bhb_init_adc_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_adc), MP_ROM_PTR(&_bhb_read_adc_obj) },
};

STATIC MP_DEFINE_CONST_DICT(_bhb_module_globals, _bhb_module_globals_table);

const mp_obj_module_t _bhb_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&_bhb_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__bhb, _bhb_user_cmodule, MODULE_BHB_ENABLED);
