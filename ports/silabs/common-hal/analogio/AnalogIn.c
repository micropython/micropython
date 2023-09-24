/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/runtime.h"
#include "common-hal/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/translate/translate.h"
#include "em_cmu.h"
#include "em_iadc.h"

// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ 20000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ 10000000      // CLK_ADC - 10 MHz max in normal mode

// Number of scan channels
#define NUM_INPUTS 8

STATIC uint8_t num_current_input = 0;
STATIC volatile uint16_t scan_result[NUM_INPUTS];
STATIC volatile uint8_t scan_flag = 0;
STATIC IADC_ScanTable_t init_scan_table = IADC_SCANTABLE_DEFAULT;  // Scan Table

// Construct analogin pin. This function is called when init AnalogIn
void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {

    uint8_t adc_index;
    if (self->pin == NULL) {
        self->id = NUM_INPUTS + 1;
        for (adc_index = 0; adc_index < NUM_INPUTS; adc_index++) {
            if (init_scan_table.entries[adc_index].includeInScan == false) {
                self->id = adc_index;
                self->pin = pin;
                init_scan_table.entries[adc_index].includeInScan = true;
                init_scan_table.entries[adc_index].negInput = iadcNegInputGnd;
                init_scan_table.entries[adc_index].posInput
                    = IADC_portPinToPosInput(self->pin->port, self->pin->number);
                num_current_input++;
                break;
            }
        }
    }

    if (self->id == NUM_INPUTS + 1) {
        mp_raise_ValueError(translate("ADC busy pin"));
    }

    // Declare init structs
    IADC_Init_t init = IADC_INIT_DEFAULT;
    IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
    IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;

    CMU_ClockEnable(cmuClock_IADC0, true);

    // Select clock for IADC
    CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

    // Modify init structures and initialize
    init.warmup = iadcWarmupKeepWarm;

    // Set the HFSCLK prescale value here
    init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

    // Configuration 0 is used by both scan and single conversions by
    // default.  Use internal bandgap as the reference and specify the
    // reference voltage in mV.
    // Resolution is not configurable directly but is based on the
    // selected oversampling ratio (osrHighSpeed), which defaults to
    // 2x and generates 12-bit results.
    initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
    initAllConfigs.configs[0].vRef = 3300;
    initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
    initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain1x;

    // Divide CLK_SRC_ADC to set the CLK_ADC frequency
    initAllConfigs.configs[0].adcClkPrescale
        = IADC_calcAdcClkPrescale(IADC0,
        CLK_ADC_FREQ,
        0,
        iadcCfgModeNormal,
        init.srcClkPrescale);


    // Set the SCANFIFODVL flag when there are 8 entries in the scan
    // FIFO.  Note that in this example, the interrupt associated with
    // the SCANFIFODVL flag in the IADC_IF register is not used.
    // Similarly, the fifoDmaWakeup member of the initScan structure
    // is left at its default setting of false, so LDMA service is not
    // requested when the FIFO holds the specified number of samples.
    initScan.dataValidLevel = _IADC_SCANFIFOCFG_DVL_VALID8;

    // Tag FIFO entry with scan table entry id
    initScan.showId = true;

    // Initialize IADC
    IADC_init(IADC0, &init, &initAllConfigs);

    // Initialize scan
    IADC_initScan(IADC0, &initScan, &init_scan_table);

    if (self->pin->port == gpioPortA) {
        GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0;
        GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD0_ADC0;
    } else if (self->pin->port == gpioPortB) {
        GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN0_ADC0;
        GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD0_ADC0;
    } else {
        GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;
        GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0;
    }

    // Clear any previous interrupt flags
    IADC_clearInt(IADC0, _IADC_IF_MASK);

    // Enable Scan interrupts
    IADC_enableInt(IADC0, IADC_IEN_SCANTABLEDONE);

    // Enable ADC interrupts
    NVIC_ClearPendingIRQ(IADC_IRQn);
    NVIC_EnableIRQ(IADC_IRQn);

    common_hal_mcu_pin_claim(pin);
}

// Check obj is deinited or not
bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

// Deinit a analogin obj
void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (num_current_input > 0) {
        num_current_input--;
        if (num_current_input == 0) {
            IADC_reset(IADC0);
        }
    }
    init_scan_table.entries[self->id].includeInScan = false;
    init_scan_table.entries[self->id].posInput = iadcPosInputGnd;
    scan_result[self->id] = 0;
    common_hal_reset_pin(self->pin);
    self->pin = NULL;
}

// IADC Handler to read adc value
void IADC_IRQHandler(void) {
    IADC_Result_t result = {0, 0};

    // While the FIFO count is non-zero
    while (IADC_getScanFifoCnt(IADC0)) {
        // Pull a scan result from the FIFO
        result = IADC_pullScanFifoResult(IADC0);
        scan_result[result.id] = result.data;
        scan_result[result.id] *= 16;
    }
    scan_flag = 1;
    IADC_clearInt(IADC0, IADC_IF_SCANTABLEDONE);
}

//  Get adc value, use IADC_IRQHandler
//  adc value 0 - 65535
uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Start scan
    IADC_command(IADC0, iadcCmdStartScan);

    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t current_ticks = start_ticks;

    // Busy-wait until timeout or until we've read enough chars.
    while (current_ticks - start_ticks <= 1000) {
        current_ticks = supervisor_ticks_ms64();
        if (scan_flag == 1) {
            scan_flag = 0;
            break;
        }
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }
    }

    uint16_t ret = scan_result[self->id];
    scan_result[self->id] = 0;
    return ret;
}

// Get adc ref value
float common_hal_analogio_analogin_get_reference_voltage
    (analogio_analogin_obj_t *self) {
    return 3.3f;
}
