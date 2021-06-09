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
#include "shared-bindings/microcontroller/ResetReason.h"

#include "samd/adc.h"

#include "peripheral_clk_config.h"

#define ADC_TEMP_SAMPLE_LENGTH 4
#define INT1V_VALUE_FLOAT 1.0
#define INT1V_DIVIDER_1000 1000.0
#define ADC_12BIT_FULL_SCALE_VALUE_FLOAT 4095.0

// channel argument (ignored in calls below)
#define IGNORED_CHANNEL 0

// Decimal to fraction conversion. (adapted from ASF sample).
STATIC float convert_dec_to_frac(uint8_t val) {
    float float_val = (float)val;
    if (val < 10) {
        return float_val / 10.0;
    } else if (val < 100) {
        return float_val / 100.0;
    } else {
        return float_val / 1000.0;
    }
}

// Extract the production calibration data information from NVM (adapted from ASF sample),
// then calculate the temperature
#ifdef SAMD21
STATIC float calculate_temperature(uint16_t raw_value) {
    volatile uint32_t val1;    /* Temperature Log Row Content first 32 bits */
    volatile uint32_t val2;    /* Temperature Log Row Content another 32 bits */
    uint8_t room_temp_val_int; /* Integer part of room temperature in °C */
    uint8_t room_temp_val_dec; /* Decimal part of room temperature in °C */
    uint8_t hot_temp_val_int;  /* Integer part of hot temperature in °C */
    uint8_t hot_temp_val_dec;  /* Decimal part of hot temperature in °C */
    int8_t room_int1v_val;     /* internal 1V reference drift at room temperature */
    int8_t hot_int1v_val;      /* internal 1V reference drift at hot temperature*/

    float tempR;       // Production Room temperature
    float tempH;       // Production Hot temperature
    float INT1VR;      // Room temp 2's complement of the internal 1V reference value
    float INT1VH;      // Hot temp 2's complement of the internal 1V reference value
    uint16_t ADCR;     // Production Room temperature ADC value
    uint16_t ADCH;     // Production Hot temperature ADC value
    float VADCR;       // Room temperature ADC voltage
    float VADCH;       // Hot temperature ADC voltage

    uint32_t *temp_log_row_ptr = (uint32_t *)NVMCTRL_TEMP_LOG;

    val1 = *temp_log_row_ptr;
    temp_log_row_ptr++;
    val2 = *temp_log_row_ptr;

    room_temp_val_int = (uint8_t)((val1 & FUSES_ROOM_TEMP_VAL_INT_Msk) >> FUSES_ROOM_TEMP_VAL_INT_Pos);
    room_temp_val_dec = (uint8_t)((val1 & FUSES_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_ROOM_TEMP_VAL_DEC_Pos);

    hot_temp_val_int = (uint8_t)((val1 & FUSES_HOT_TEMP_VAL_INT_Msk) >> FUSES_HOT_TEMP_VAL_INT_Pos);
    hot_temp_val_dec = (uint8_t)((val1 & FUSES_HOT_TEMP_VAL_DEC_Msk) >> FUSES_HOT_TEMP_VAL_DEC_Pos);

    room_int1v_val = (int8_t)((val1 & FUSES_ROOM_INT1V_VAL_Msk) >> FUSES_ROOM_INT1V_VAL_Pos);
    hot_int1v_val = (int8_t)((val2 & FUSES_HOT_INT1V_VAL_Msk) >> FUSES_HOT_INT1V_VAL_Pos);

    ADCR = (uint16_t)((val2 & FUSES_ROOM_ADC_VAL_Msk) >> FUSES_ROOM_ADC_VAL_Pos);
    ADCH = (uint16_t)((val2 & FUSES_HOT_ADC_VAL_Msk) >> FUSES_HOT_ADC_VAL_Pos);

    tempR = room_temp_val_int + convert_dec_to_frac(room_temp_val_dec);
    tempH = hot_temp_val_int + convert_dec_to_frac(hot_temp_val_dec);

    INT1VR = 1 - ((float)room_int1v_val / INT1V_DIVIDER_1000);
    INT1VH = 1 - ((float)hot_int1v_val / INT1V_DIVIDER_1000);

    VADCR = ((float)ADCR * INT1VR) / ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
    VADCH = ((float)ADCH * INT1VH) / ADC_12BIT_FULL_SCALE_VALUE_FLOAT;

    float VADC;      /* Voltage calculation using ADC result for Coarse Temp calculation */
    float VADCM;     /* Voltage calculation using ADC result for Fine Temp calculation. */
    float INT1VM;    /* Voltage calculation for reality INT1V value during the ADC conversion */

    VADC = ((float)raw_value * INT1V_VALUE_FLOAT) / ADC_12BIT_FULL_SCALE_VALUE_FLOAT;

    // Hopefully compiler will remove common subepxressions here.

    // calculate fine temperature using Equation1 and Equation
    // 1b as mentioned in data sheet section "Temperature Sensor Characteristics"
    // of Electrical Characteristics. (adapted from ASF sample code).
    // Coarse Temp Calculation by assume INT1V=1V for this ADC conversion
    float coarse_temp = tempR + (((tempH - tempR) / (VADCH - VADCR)) * (VADC - VADCR));

    // Calculation to find the real INT1V value during the ADC conversion
    INT1VM = INT1VR + (((INT1VH - INT1VR) * (coarse_temp - tempR)) / (tempH - tempR));

    VADCM = ((float)raw_value * INT1VM) / ADC_12BIT_FULL_SCALE_VALUE_FLOAT;

    // Fine Temp Calculation by replace INT1V=1V by INT1V = INT1Vm for ADC conversion
    float fine_temp = tempR + (((tempH - tempR) / (VADCH - VADCR)) * (VADCM - VADCR));

    return fine_temp;
}
#endif // SAMD21

#ifdef SAM_D5X_E5X
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
