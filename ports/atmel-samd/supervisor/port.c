/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "boards/board.h"
#include "supervisor/port.h"

// ASF 4
#include "atmel_start_pins.h"
#include "hal/include/hal_delay.h"
#include "hal/include/hal_gpio.h"
#include "hal/include/hal_init.h"
#include "hal/include/hal_usb_device.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "hpl/pm/hpl_pm_base.h"

#ifdef SAMD21
#include "hri/hri_pm_d21.h"
#endif
#ifdef SAMD51
#include "hri/hri_rstc_d51.h"
#endif

#include "common-hal/analogio/AnalogIn.h"
#include "common-hal/analogio/AnalogOut.h"
#include "common-hal/audiobusio/PDMIn.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "common-hal/audioio/AudioOut.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PWMOut.h"
#include "common-hal/rtc/RTC.h"
#include "common-hal/usb_hid/Device.h"
#include "shared-bindings/rtc/__init__.h"
#include "clocks.h"
#include "events.h"
#include "shared_dma.h"
#include "tick.h"

#ifdef CIRCUITPY_GAMEPAD_TICKS
#include "shared-module/gamepad/__init__.h"
#endif

extern volatile bool mp_msc_enabled;

#if defined(SAMD21) && defined(ENABLE_MICRO_TRACE_BUFFER)
// Stores 2 ^ TRACE_BUFFER_MAGNITUDE_PACKETS packets.
// 7 -> 128 packets
#define TRACE_BUFFER_MAGNITUDE_PACKETS 7
// Size in uint32_t. Two per packet.
#define TRACE_BUFFER_SIZE (1 << (TRACE_BUFFER_MAGNITUDE_PACKETS + 1))
// Size in bytes. 4 bytes per uint32_t.
#define TRACE_BUFFER_SIZE_BYTES (TRACE_BUFFER_SIZE << 2)
__attribute__((__aligned__(TRACE_BUFFER_SIZE_BYTES))) uint32_t mtb[TRACE_BUFFER_SIZE];
#endif

safe_mode_t port_init(void) {
#if defined(SAMD21)
    #ifdef ENABLE_MICRO_TRACE_BUFFER
        REG_MTB_POSITION = ((uint32_t) (mtb - REG_MTB_BASE)) & 0xFFFFFFF8;
        REG_MTB_FLOW = (((uint32_t) mtb - REG_MTB_BASE) + TRACE_BUFFER_SIZE_BYTES) & 0xFFFFFFF8;
        REG_MTB_MASTER = 0x80000000 + (TRACE_BUFFER_MAGNITUDE_PACKETS - 1);
    #else
        // Triple check that the MTB is off. Switching between debug and non-debug
        // builds can leave it set over reset and wreak havok as a result.
        REG_MTB_MASTER = 0x00000000 + 6;
    #endif
#endif


// On power on start or external reset, set _ezero to the canary word. If it
// gets killed, we boot in safe mode. _ezero is the boundary between statically
// allocated memory including the fixed MicroPython heap and the stack. If either
// misbehaves, the canary will not be intact after soft reset.
#ifdef CIRCUITPY_CANARY_WORD
#ifdef SAMD21
    bool power_on_or_external_reset = hri_pm_get_RCAUSE_POR_bit(PM) || hri_pm_get_RCAUSE_EXT_bit(PM);
    bool system_reset = hri_pm_get_RCAUSE_SYST_bit(PM);
#endif
#ifdef SAMD51
    bool power_on_or_external_reset = hri_rstc_get_RCAUSE_POR_bit(RSTC) || hri_rstc_get_RCAUSE_EXT_bit(RSTC);
    bool system_reset = hri_rstc_get_RCAUSE_SYST_bit(RSTC);
#endif
   if (power_on_or_external_reset) {
        _ezero = CIRCUITPY_CANARY_WORD;
    } else if (system_reset) {
        // If we're starting from a system reset we're likely coming from the
        // bootloader or hard fault handler. If we're coming from the handler
        // the canary will be CIRCUITPY_SAFE_RESTART_WORD and we don't want to
        // revive the canary so that a second hard fault won't restart. Resets
        // from anywhere else are ok.
        if (_ezero == CIRCUITPY_SAFE_RESTART_WORD) {
            _ezero = ~CIRCUITPY_CANARY_WORD;
        } else {
            _ezero = CIRCUITPY_CANARY_WORD;
        }
    }
#endif

    init_mcu();

    board_init();

    // Configure millisecond timer initialization.
    tick_init();
    rtc_init();

    // Init the nvm controller.
    // struct nvm_config config_nvm;
    // nvm_get_config_defaults(&config_nvm);
    // config_nvm.manual_page_write = false;
    // nvm_set_config(&config_nvm);

    init_shared_dma();
    #ifdef CIRCUITPY_CANARY_WORD
    // Run in safe mode if the canary is corrupt.
    if (_ezero != CIRCUITPY_CANARY_WORD) {
        return HARD_CRASH;
    }
    #endif

    // if (PM->RCAUSE.bit.BOD33 == 1 || PM->RCAUSE.bit.BOD12 == 1) {
    //     return BROWNOUT;
    // }

    if (board_requests_safe_mode()) {
        return USER_SAFE_MODE;
    }

    return NO_SAFE_MODE;
}

void reset_port(void) {
    // Reset all SERCOMs except the ones being used by on-board devices.
    Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
#ifdef SPI_FLASH_SERCOM
        if (sercom_instances[i] == SPI_FLASH_SERCOM) {
            continue;
        }
#endif
#ifdef MICROPY_HW_APA102_SERCOM
        if (sercom_instances[i] == MICROPY_HW_APA102_SERCOM) {
            continue;
        }
#endif
        // SWRST is same for all modes of SERCOMs.
        sercom_instances[i]->SPI.CTRLA.bit.SWRST = 1;
    }

#ifdef EXPRESS_BOARD
    audioout_reset();
    #if !defined(__SAMD51G19A__) && !defined(__SAMD51G18A__)
    i2sout_reset();
    #endif
    audio_dma_reset();
//     touchin_reset();
    //pdmin_reset();
#endif
    pulsein_reset();
    pulseout_reset();
    pwmout_reset();
    rtc_reset();

    reset_gclks();

    analogin_reset();

#ifdef CIRCUITPY_GAMEPAD_TICKS
    gamepad_reset();
#endif

    analogout_reset();

    reset_event_system();

    reset_all_pins();

    // Set up debugging pins after reset_all_pins().

    // Uncomment to init PIN_PA17 for debugging.
    // struct port_config pin_conf;
    // port_get_config_defaults(&pin_conf);
    //
    // pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    // port_pin_set_config(MICROPY_HW_LED1, &pin_conf);
    // port_pin_set_output_level(MICROPY_HW_LED1, false);

    // Output clocks for debugging.
    // not supported by SAMD51G; uncomment for SAMD51J or update for 51G
    // #ifdef SAMD51
    // gpio_set_pin_function(PIN_PA10, GPIO_PIN_FUNCTION_M); // GCLK4, D3
    // gpio_set_pin_function(PIN_PA11, GPIO_PIN_FUNCTION_M); // GCLK5, A4
    // gpio_set_pin_function(PIN_PB14, GPIO_PIN_FUNCTION_M); // GCLK0, D5
    // gpio_set_pin_function(PIN_PB15, GPIO_PIN_FUNCTION_M); // GCLK1, D6
    // #endif

    usb_hid_reset();

// #ifdef CALIBRATE_CRYSTALLESS
//     // If we are on USB lets double check our fine calibration for the clock and
//     // save the new value if its different enough.
//     if (mp_msc_enabled) {
//         SYSCTRL->DFLLSYNC.bit.READREQ = 1;
//         uint16_t saved_calibration = 0x1ff;
//         if (strcmp((char*) INTERNAL_CIRCUITPY_CONFIG_START_ADDR, "CIRCUITPYTHON1") == 0) {
//             saved_calibration = ((uint16_t *) INTERNAL_CIRCUITPY_CONFIG_START_ADDR)[8];
//         }
//         while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0) {
//             // TODO(tannewt): Run the mass storage stuff if this takes a while.
//         }
//         int16_t current_calibration = SYSCTRL->DFLLVAL.bit.FINE;
//         if (abs(current_calibration - saved_calibration) > 10) {
//             enum status_code error_code;
//             uint8_t page_buffer[NVMCTRL_ROW_SIZE];
//             for (int i = 0; i < NVMCTRL_ROW_PAGES; i++) {
//                 do
//                 {
//                     error_code = nvm_read_buffer(INTERNAL_CIRCUITPY_CONFIG_START_ADDR + i * NVMCTRL_PAGE_SIZE,
//                                                  page_buffer + i * NVMCTRL_PAGE_SIZE,
//                                                  NVMCTRL_PAGE_SIZE);
//                 } while (error_code == STATUS_BUSY);
//             }
//             // If this is the first write, include the header.
//             if (strcmp((char*) page_buffer, "CIRCUITPYTHON1") != 0) {
//                 memcpy(page_buffer, "CIRCUITPYTHON1", 15);
//             }
//             // First 16 bytes (0-15) are ID. Little endian!
//             page_buffer[16] = current_calibration & 0xff;
//             page_buffer[17] = current_calibration >> 8;
//             do
//             {
//                 error_code = nvm_erase_row(INTERNAL_CIRCUITPY_CONFIG_START_ADDR);
//             } while (error_code == STATUS_BUSY);
//             for (int i = 0; i < NVMCTRL_ROW_PAGES; i++) {
//                 do
//                 {
//                     error_code = nvm_write_buffer(INTERNAL_CIRCUITPY_CONFIG_START_ADDR + i * NVMCTRL_PAGE_SIZE,
//                                                   page_buffer + i * NVMCTRL_PAGE_SIZE,
//                                                   NVMCTRL_PAGE_SIZE);
//                 } while (error_code == STATUS_BUSY);
//             }
//         }
//     }
// #endif
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void HardFault_Handler(void)
{
    while (true) {
        asm("");
    }
    for (uint32_t i = 0; i < 100000; i++) {
        asm("noop;");
    }
}
