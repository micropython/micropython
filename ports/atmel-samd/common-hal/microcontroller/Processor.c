/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Includes code from ASF sample code adc_temp.h and adc_temp.c,
 * and so includes this license:
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>

#include "py/mphal.h"
#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "samd/adc.h"

#include "peripheral_clk_config.h"

#define ADC_TEMP_SAMPLE_LENGTH 4
#define INT1V_VALUE_FLOAT MICROPY_FLOAT_CONST(1.0)
#define INT1V_DIVIDER_1000 MICROPY_FLOAT_CONST(1000.0)
#define ADC_12BIT_FULL_SCALE_VALUE_FLOAT MICROPY_FLOAT_CONST(4095.0)

// channel argument (ignored in calls below)
#define IGNORED_CHANNEL 0


// Extract the production calibration data information from NVM (adapted from ASF sample),
// then calculate the temperature
//
// This code performs almost all operations with scaled integers.  For
// instance, tempR is in units of 1/10°C, INT1VR is in units of 1mV, etc,
// This is important to reduce the code size of the function. The effect on
// precision is a ~.9°C difference vs the floating point algorithm on an
// approximate 0..60°C range with a difference of ~.5°C at 25°C. When the fine
// calculation step is skipped, the additional error approximately doubles.
//
// To save code size, rounding is neglected. However, trying to add back rounding
// (by computing (a + b/2) / b instead of just a / b) actually didn't help
// accuracy anyway.
#ifdef SAMD21
STATIC float calculate_temperature(uint16_t raw_value) {
    uint32_t val1;    /* Temperature Log Row Content first 32 bits */
    uint32_t val2;    /* Temperature Log Row Content another 32 bits */
    int room_temp_val_int; /* Integer part of room temperature in °C */
    int room_temp_val_dec; /* Decimal part of room temperature in °C */
    int hot_temp_val_int;  /* Integer part of hot temperature in °C */
    int hot_temp_val_dec;  /* Decimal part of hot temperature in °C */
    int room_int1v_val;     /* internal 1V reference drift at room temperature */
    int hot_int1v_val;      /* internal 1V reference drift at hot temperature*/

    uint32_t *temp_log_row_ptr = (uint32_t *)NVMCTRL_TEMP_LOG;

    val1 = *temp_log_row_ptr;
    temp_log_row_ptr++;
    val2 = *temp_log_row_ptr;

    room_temp_val_int = ((val1 & FUSES_ROOM_TEMP_VAL_INT_Msk) >> FUSES_ROOM_TEMP_VAL_INT_Pos);
    room_temp_val_dec = ((val1 & FUSES_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_ROOM_TEMP_VAL_DEC_Pos);

    hot_temp_val_int = ((val1 & FUSES_HOT_TEMP_VAL_INT_Msk) >> FUSES_HOT_TEMP_VAL_INT_Pos);
    hot_temp_val_dec = ((val1 & FUSES_HOT_TEMP_VAL_DEC_Msk) >> FUSES_HOT_TEMP_VAL_DEC_Pos);

    // necessary casts: must interpret 8 bits as signed
    room_int1v_val = (int8_t)((val1 & FUSES_ROOM_INT1V_VAL_Msk) >> FUSES_ROOM_INT1V_VAL_Pos);
    hot_int1v_val = (int8_t)((val2 & FUSES_HOT_INT1V_VAL_Msk) >> FUSES_HOT_INT1V_VAL_Pos);

    int ADCR = ((val2 & FUSES_ROOM_ADC_VAL_Msk) >> FUSES_ROOM_ADC_VAL_Pos);
    int ADCH = ((val2 & FUSES_HOT_ADC_VAL_Msk) >> FUSES_HOT_ADC_VAL_Pos);

    int tempR = 10 * room_temp_val_int + room_temp_val_dec;
    int tempH = 10 * hot_temp_val_int + hot_temp_val_dec;

    int INT1VR = 1000 - room_int1v_val;
    int INT1VH = 1000 - hot_int1v_val;

    int VADCR = ADCR * INT1VR;
    int VADCH = ADCH * INT1VH;

    int VADC = raw_value * 1000;

    // Hopefully compiler will remove common subepxressions here.

    // calculate fine temperature using Equation1 and Equation
    // 1b as mentioned in data sheet section "Temperature Sensor Characteristics"
    // of Electrical Characteristics. (adapted from ASF sample code).
    // Coarse Temp Calculation by assume INT1V=1V for this ADC conversion
    int coarse_temp = tempR + (tempH - tempR) * (VADC - VADCR) / (VADCH - VADCR);

    #if CIRCUITPY_FULL_BUILD
    // Calculation to find the real INT1V value during the ADC conversion
    int INT1VM;    /* Voltage calculation for reality INT1V value during the ADC conversion */

    INT1VM = INT1VR + (((INT1VH - INT1VR) * (coarse_temp - tempR)) / (tempH - tempR));

    int VADCM = raw_value * INT1VM;

    // Fine Temp Calculation by replace INT1V=1V by INT1V = INT1Vm for ADC conversion
    float fine_temp = tempR + (((tempH - tempR) * (VADCM - VADCR)) / (VADCH - VADCR));

    return fine_temp / 10;
    #else
    return coarse_temp / 10.;
    #endif
}
#endif // SAMD21

#ifdef SAM_D5X_E5X
// Decimal to fraction conversion. (adapted from ASF sample).
STATIC float convert_dec_to_frac(uint8_t val) {
    return val / MICROPY_FLOAT_CONST(10.);
}
STATIC float calculate_temperature(uint16_t TP, uint16_t TC) {
    uint32_t TLI = (*(uint32_t *)FUSES_ROOM_TEMP_VAL_INT_ADDR & FUSES_ROOM_TEMP_VAL_INT_Msk) >> FUSES_ROOM_TEMP_VAL_INT_Pos;
    uint32_t TLD = (*(uint32_t *)FUSES_ROOM_TEMP_VAL_DEC_ADDR & FUSES_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_ROOM_TEMP_VAL_DEC_Pos;
    float TL = TLI + convert_dec_to_frac(TLD);

    uint32_t THI = (*(uint32_t *)FUSES_HOT_TEMP_VAL_INT_ADDR & FUSES_HOT_TEMP_VAL_INT_Msk) >> FUSES_HOT_TEMP_VAL_INT_Pos;
    uint32_t THD = (*(uint32_t *)FUSES_HOT_TEMP_VAL_DEC_ADDR & FUSES_HOT_TEMP_VAL_DEC_Msk) >> FUSES_HOT_TEMP_VAL_DEC_Pos;
    float TH = THI + convert_dec_to_frac(THD);

    uint16_t VPL = (*(uint32_t *)FUSES_ROOM_ADC_VAL_PTAT_ADDR & FUSES_ROOM_ADC_VAL_PTAT_Msk) >> FUSES_ROOM_ADC_VAL_PTAT_Pos;
    uint16_t VPH = (*(uint32_t *)FUSES_HOT_ADC_VAL_PTAT_ADDR & FUSES_HOT_ADC_VAL_PTAT_Msk) >> FUSES_HOT_ADC_VAL_PTAT_Pos;

    uint16_t VCL = (*(uint32_t *)FUSES_ROOM_ADC_VAL_CTAT_ADDR & FUSES_ROOM_ADC_VAL_CTAT_Msk) >> FUSES_ROOM_ADC_VAL_CTAT_Pos;
    uint16_t VCH = (*(uint32_t *)FUSES_HOT_ADC_VAL_CTAT_ADDR & FUSES_HOT_ADC_VAL_CTAT_Msk) >> FUSES_HOT_ADC_VAL_CTAT_Pos;

    // From SAMD51 datasheet: section 45.6.3.1 (page 1327).
    return (TL * VPH * TC - VPL * TH * TC - TL * VCH * TP + TH * VCL * TP) / (VCL * TP - VCH * TP - VPL * TC + VPH * TC);
}
#endif // SAMD51

float common_hal_mcu_processor_get_temperature(void) {
    struct adc_sync_descriptor adc;

    static Adc *adc_insts[] = ADC_INSTS;
    samd_peripherals_adc_setup(&adc, adc_insts[0]);

    #ifdef SAMD21
    // The parameters chosen here are from the temperature example in:
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // That note also recommends in general:
    // "Discard the first conversion result whenever there is a change
    // in ADC configuration like voltage reference / ADC channel change."

    adc_sync_set_resolution(&adc, ADC_CTRLB_RESSEL_12BIT_Val);
    adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INT1V_Val);
    // Channel arg is ignored.
    adc_sync_enable_channel(&adc, IGNORED_CHANNEL);
    adc_sync_set_inputs(&adc,
        ADC_INPUTCTRL_MUXPOS_TEMP_Val,                   // pos_input
        ADC_INPUTCTRL_MUXNEG_GND_Val,                    // neg_input
        IGNORED_CHANNEL);                                // channel (ignored)

    hri_adc_write_CTRLB_PRESCALER_bf(adc.device.hw, ADC_CTRLB_PRESCALER_DIV32_Val);
    hri_adc_write_SAMPCTRL_SAMPLEN_bf(adc.device.hw, ADC_TEMP_SAMPLE_LENGTH);

    hri_sysctrl_set_VREF_TSEN_bit(SYSCTRL);

    // Oversample and decimate. A higher samplenum produces a more stable result.
    hri_adc_write_AVGCTRL_SAMPLENUM_bf(adc.device.hw, ADC_AVGCTRL_SAMPLENUM_4_Val);
    hri_adc_write_AVGCTRL_ADJRES_bf(adc.device.hw, 2);

    volatile uint16_t value;

    // Read twice and discard first result, as recommended in section 14 of
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // "Discard the first conversion result whenever there is a change in ADC configuration
    // like voltage reference / ADC channel change"
    // Empirical observation shows the first reading is quite different than subsequent ones.

    // Channel arg is ignored.
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&value), 2);
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&value), 2);

    adc_sync_deinit(&adc);
    return calculate_temperature(value);
    #endif // SAMD21

    #ifdef SAM_D5X_E5X
    adc_sync_set_resolution(&adc, ADC_CTRLB_RESSEL_12BIT_Val);
    // Using INTVCC0 as the reference voltage.
    // INTVCC1 seems to read a little high.
    // INTREF doesn't work: ADC hangs BUSY. It's supposed to work, but does not.
    // The SAME54 example from Atmel START implicitly uses INTREF.
    adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INTVCC0_Val);

    hri_supc_set_VREF_ONDEMAND_bit(SUPC);
    // Enable temperature sensor.
    hri_supc_set_VREF_TSEN_bit(SUPC);
    hri_supc_set_VREF_VREFOE_bit(SUPC);

    // Channel arg is ignored.
    adc_sync_enable_channel(&adc, IGNORED_CHANNEL);
    adc_sync_set_inputs(&adc,
        ADC_INPUTCTRL_MUXPOS_PTAT_Val,                   // pos_input
        ADC_INPUTCTRL_MUXNEG_GND_Val,                    // neg_input
        IGNORED_CHANNEL);                                // channel (ignored)

    // Read both temperature sensors.
    volatile uint16_t ptat;
    volatile uint16_t ctat;

    // Read twice for stability (necessary?).
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&ptat), 2);
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&ptat), 2);

    adc_sync_set_inputs(&adc,
        ADC_INPUTCTRL_MUXPOS_CTAT_Val,                   // pos_input
        ADC_INPUTCTRL_MUXNEG_GND_Val,                    // neg_input
        IGNORED_CHANNEL);                                // channel (ignored)

    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&ctat), 2);
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&ctat), 2);

    // Turn off temp sensor.
    hri_supc_clear_VREF_TSEN_bit(SUPC);

    adc_sync_deinit(&adc);
    return calculate_temperature(ptat, ctat);
    #endif // SAMD51
}

float common_hal_mcu_processor_get_voltage(void) {
    struct adc_sync_descriptor adc;

    static Adc *adc_insts[] = ADC_INSTS;
    samd_peripherals_adc_setup(&adc, adc_insts[0]);

    #ifdef SAMD21
    adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INT1V_Val);
    #endif

    #ifdef SAM_D5X_E5X
    hri_supc_clear_VREF_ONDEMAND_bit(SUPC);
    hri_supc_set_VREF_SEL_bf(SUPC, SUPC_VREF_SEL_1V0_Val);
    hri_supc_set_VREF_VREFOE_bit(SUPC);

    adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INTREF_Val);

    // On some processor samples, the ADC will hang trying to read the voltage. A simple
    // delay after setting the SUPC bits seems to fix things. This appears to be due to VREFOE
    // startup time. There is no synchronization bit to check.
    // See https://community.atmel.com/forum/samd51-using-intref-adc-voltage-reference
    mp_hal_delay_ms(1);
    #endif

    adc_sync_set_resolution(&adc, ADC_CTRLB_RESSEL_12BIT_Val);
    // Channel arg is ignored.
    adc_sync_set_inputs(&adc,
        ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC_Val,                     // IOVCC/4 (nominal 3.3V/4)
        ADC_INPUTCTRL_MUXNEG_GND_Val,                             // neg_input
        IGNORED_CHANNEL);                                         // channel (ignored).
    adc_sync_enable_channel(&adc, IGNORED_CHANNEL);

    volatile uint16_t reading;

    // Channel arg is ignored.
    // Read twice and discard first result, as recommended in section 14 of
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // "Discard the first conversion result whenever there is a change in ADC configuration
    // like voltage reference / ADC channel change"
    // Empirical observation shows the first reading is quite different than subsequent ones.
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&reading), 2);
    adc_sync_read_channel(&adc, IGNORED_CHANNEL, ((uint8_t *)&reading), 2);

    adc_sync_deinit(&adc);
    // Multiply by 4 to compensate for SCALEDIOVCC division by 4.
    return (reading / 4095.0f) * 4.0f;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    // TODO(tannewt): Determine this dynamically.
    return CONF_CPU_FREQUENCY;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    #ifdef SAMD21
    uint32_t *id_addresses[4] = {(uint32_t *)0x0080A00C, (uint32_t *)0x0080A040,
                                 (uint32_t *)0x0080A044, (uint32_t *)0x0080A048};
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t *id_addresses[4] = {(uint32_t *)0x008061FC, (uint32_t *)0x00806010,
                                 (uint32_t *)0x00806014, (uint32_t *)0x00806018};
    #endif

    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            raw_id[4 * i + k] = (*(id_addresses[i]) >> k * 8) & 0xff;
        }
    }
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_UNKNOWN;
}
