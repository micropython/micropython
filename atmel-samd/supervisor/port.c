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

#include "tick.h"

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

// // On power on start or external reset, set _ezero to the canary word. If it
// // gets killed, we boot in safe mod. _ezero is the boundary between statically
// // allocated memory including the fixed MicroPython heap and the stack. If either
// // misbehaves, the canary will not be in tact after soft reset.
// #ifdef CIRCUITPY_CANARY_WORD
//     if (PM->RCAUSE.bit.POR == 1 || PM->RCAUSE.bit.EXT == 1) {
//         _ezero = CIRCUITPY_CANARY_WORD;
//     } else if (PM->RCAUSE.bit.SYST == 1) {
//         // If we're starting from a system reset we're likely coming from the
//         // bootloader or hard fault handler. If we're coming from the handler
//         // the canary will be CIRCUITPY_SAFE_RESTART_WORD and we don't want to
//         // revive the canary so that a second hard fault won't restart. Resets
//         // from anywhere else are ok.
//         if (_ezero == CIRCUITPY_SAFE_RESTART_WORD) {
//             _ezero = ~CIRCUITPY_CANARY_WORD;
//         } else {
//             _ezero = CIRCUITPY_CANARY_WORD;
//         }
//     }
// #endif
//
//     load_serial_number();

    init_mcu();

    board_init();

    // Configure millisecond timer initialization.
    tick_init();

    // Uncomment to init PIN_PA17 for debugging.
    // struct port_config pin_conf;
    // port_get_config_defaults(&pin_conf);
    //
    // pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    // port_pin_set_config(MICROPY_HW_LED1, &pin_conf);
    // port_pin_set_output_level(MICROPY_HW_LED1, false);

    // Output clocks for debugging.
    #ifdef SAMD51
    gpio_set_pin_function(PIN_PA10, GPIO_PIN_FUNCTION_M); // GCLK4, D3
    gpio_set_pin_function(PIN_PA11, GPIO_PIN_FUNCTION_M); // GCLK5, A4
    gpio_set_pin_function(PIN_PB14, GPIO_PIN_FUNCTION_M); // GCLK0, D5
    gpio_set_pin_function(PIN_PB15, GPIO_PIN_FUNCTION_M); // GCLK1, D6
    #endif

    // Init the nvm controller.
    // struct nvm_config config_nvm;
    // nvm_get_config_defaults(&config_nvm);
    // config_nvm.manual_page_write = false;
    // nvm_set_config(&config_nvm);

    // init_shared_dma();
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

    // #if CIRCUITPY_INTERNAL_NVM_SIZE > 0
    // // Upgrade the nvm flash to include one sector for eeprom emulation.
    // struct nvm_fusebits fuses;
    // if (nvm_get_fuses(&fuses) == STATUS_OK &&
    //         fuses.eeprom_size == NVM_EEPROM_EMULATOR_SIZE_0) {
    //     #ifdef INTERNAL_FLASH_FS
    //     // Shift the internal file system up one row.
    //     for (uint8_t row = 0; row < TOTAL_INTERNAL_FLASH_SIZE / NVMCTRL_ROW_SIZE; row++) {
    //         uint32_t new_row_address = INTERNAL_FLASH_MEM_SEG1_START_ADDR + row * NVMCTRL_ROW_SIZE;
    //         nvm_erase_row(new_row_address);
    //         nvm_write_buffer(new_row_address,
    //                          (uint8_t*) (new_row_address + CIRCUITPY_INTERNAL_EEPROM_SIZE),
    //                          NVMCTRL_ROW_SIZE);
    //     }
    //     #endif
    //     uint32_t nvm_size = CIRCUITPY_INTERNAL_NVM_SIZE;
    //     uint8_t enum_value = 6;
    //     while (nvm_size > 256 && enum_value != 255) {
    //         nvm_size /= 2;
    //         enum_value -= 1;
    //     }
    //     if (enum_value != 255 && nvm_size == 256) {
    //         // Mark the last section as eeprom now.
    //         fuses.eeprom_size = (enum nvm_eeprom_emulator_size) enum_value;
    //         nvm_set_fuses(&fuses);
    //     }
    // }
    // #endif

    return NO_SAFE_MODE;
}

void reset_port(void) {
    // Reset all SERCOMs except the one being used by the SPI flash.
//     Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;
//     for (int i = 0; i < SERCOM_INST_NUM; i++) {
//         #ifdef SPI_FLASH_SERCOM
//             if (sercom_instances[i] == SPI_FLASH_SERCOM) {
//                 continue;
//             }
//         #endif
//         #ifdef MICROPY_HW_APA102_SERCOM
//             if (sercom_instances[i] == MICROPY_HW_APA102_SERCOM) {
//                 continue;
//             }
//         #endif
//         sercom_instances[i]->SPI.CTRLA.bit.SWRST = 1;
//     }
//
// #ifdef EXPRESS_BOARD
//     audioout_reset();
//     touchin_reset();
//     pdmin_reset();
//     pulsein_reset();
//     pulseout_reset();
//     pwmout_reset();
// #endif
//
//     analogin_reset();
//
//
//     // Wait for the DAC to sync then reset.
//     while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {}
//     DAC->CTRLA.reg |= DAC_CTRLA_SWRST;
//
//     reset_all_pins();
//
//
//     usb_hid_reset();
//
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
