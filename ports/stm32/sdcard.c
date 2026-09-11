/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"

#include "sdcard.h"
#include "pin.h"
#include "pin_static_af.h"
#include "bufhelper.h"
#include "dma.h"
#include "irq.h"

#if !BUILDING_MBOOT
#include "usbd_msc_interface.h"
#else
// For mboot, act like the SDCard is always exposed via USB MSC
inline static bool usbd_msc_lu_includes_sdcard(void) {
    return true;
}
#endif // !BUILDING_MBOOT

#if MICROPY_HW_ENABLE_SDCARD || MICROPY_HW_ENABLE_MMCARD

#if defined(STM32F7) || defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32N6)

// The H7/F7/L4 have 2 SDMMC peripherals, but at the moment this driver only supports
// using one of them in a given build, selected by MICROPY_HW_SDCARD_SDMMC.

#if MICROPY_HW_SDCARD_SDMMC == 2
#define SDIO SDMMC2
#define SDMMC_IRQHandler SDMMC2_IRQHandler
#define SDMMC_CLK_ENABLE() __HAL_RCC_SDMMC2_CLK_ENABLE()
#define SDMMC_CLK_DISABLE() __HAL_RCC_SDMMC2_CLK_DISABLE()
#define SDMMC_FORCE_RESET() __HAL_RCC_SDMMC2_FORCE_RESET()
#define SDMMC_RELEASE_RESET() __HAL_RCC_SDMMC2_RELEASE_RESET()
#define SDMMC_IRQn SDMMC2_IRQn
#define SDMMC_DMA dma_SDMMC_2
#define STATIC_AF_SDCARD_CK STATIC_AF_SDMMC2_CK
#define STATIC_AF_SDCARD_CMD STATIC_AF_SDMMC2_CMD
#define STATIC_AF_SDCARD_D0 STATIC_AF_SDMMC2_D0
#define STATIC_AF_SDCARD_D1 STATIC_AF_SDMMC2_D1
#define STATIC_AF_SDCARD_D2 STATIC_AF_SDMMC2_D2
#define STATIC_AF_SDCARD_D3 STATIC_AF_SDMMC2_D3
#define STATIC_AF_SDCARD_D4 STATIC_AF_SDMMC2_D4
#define STATIC_AF_SDCARD_D5 STATIC_AF_SDMMC2_D5
#define STATIC_AF_SDCARD_D6 STATIC_AF_SDMMC2_D6
#define STATIC_AF_SDCARD_D7 STATIC_AF_SDMMC2_D7
#else
#define SDIO SDMMC1
#define SDMMC_IRQHandler SDMMC1_IRQHandler
#define SDMMC_CLK_ENABLE() __HAL_RCC_SDMMC1_CLK_ENABLE()
#define SDMMC_CLK_DISABLE() __HAL_RCC_SDMMC1_CLK_DISABLE()
#define SDMMC_FORCE_RESET() __HAL_RCC_SDMMC1_FORCE_RESET()
#define SDMMC_RELEASE_RESET() __HAL_RCC_SDMMC1_RELEASE_RESET()
#define SDMMC_IRQn SDMMC1_IRQn
#define SDMMC_DMA dma_SDIO_0
#define STATIC_AF_SDCARD_CK STATIC_AF_SDMMC1_CK
#define STATIC_AF_SDCARD_CMD STATIC_AF_SDMMC1_CMD
#define STATIC_AF_SDCARD_D0 STATIC_AF_SDMMC1_D0
#define STATIC_AF_SDCARD_D1 STATIC_AF_SDMMC1_D1
#define STATIC_AF_SDCARD_D2 STATIC_AF_SDMMC1_D2
#define STATIC_AF_SDCARD_D3 STATIC_AF_SDMMC1_D3
#define STATIC_AF_SDCARD_D4 STATIC_AF_SDMMC1_D4
#define STATIC_AF_SDCARD_D5 STATIC_AF_SDMMC1_D5
#define STATIC_AF_SDCARD_D6 STATIC_AF_SDMMC1_D6
#define STATIC_AF_SDCARD_D7 STATIC_AF_SDMMC1_D7
#endif

// The F7 & L4 series calls the peripheral SDMMC rather than SDIO, so provide some
// #defines for backwards compatibility.

#define SDIO_CLOCK_EDGE_RISING              SDMMC_CLOCK_EDGE_RISING
#define SDIO_CLOCK_EDGE_FALLING             SDMMC_CLOCK_EDGE_FALLING

#define SDIO_CLOCK_BYPASS_DISABLE           SDMMC_CLOCK_BYPASS_DISABLE
#define SDIO_CLOCK_BYPASS_ENABLE            SDMMC_CLOCK_BYPASS_ENABLE

#define SDIO_CLOCK_POWER_SAVE_DISABLE       SDMMC_CLOCK_POWER_SAVE_DISABLE
#define SDIO_CLOCK_POWER_SAVE_ENABLE        SDMMC_CLOCK_POWER_SAVE_ENABLE

#define SDIO_BUS_WIDE_1B                    SDMMC_BUS_WIDE_1B
#define SDIO_BUS_WIDE_4B                    SDMMC_BUS_WIDE_4B
#define SDIO_BUS_WIDE_8B                    SDMMC_BUS_WIDE_8B

#define SDIO_HARDWARE_FLOW_CONTROL_DISABLE  SDMMC_HARDWARE_FLOW_CONTROL_DISABLE
#define SDIO_HARDWARE_FLOW_CONTROL_ENABLE   SDMMC_HARDWARE_FLOW_CONTROL_ENABLE

#if defined(STM32H5) || defined(STM32H7) || defined(STM32N6)
#define SDIO_TRANSFER_CLK_DIV               SDMMC_HSPEED_CLK_DIV
#define SDIO_USE_GPDMA                      0
#else
#define SDIO_TRANSFER_CLK_DIV               SDMMC_TRANSFER_CLK_DIV
#define SDIO_USE_GPDMA                      1
#endif

#else

// These are definitions for F4 MCUs so there is a common macro across all MCUs.

#define SDMMC_CLK_ENABLE() __SDIO_CLK_ENABLE()
#define SDMMC_CLK_DISABLE() __SDIO_CLK_DISABLE()
#define SDMMC_IRQn SDIO_IRQn
#define SDMMC_IRQHandler SDIO_IRQHandler
#define SDMMC_DMA dma_SDIO_0
#define SDIO_USE_GPDMA 1
#define STATIC_AF_SDCARD_CK STATIC_AF_SDIO_CK
#define STATIC_AF_SDCARD_CMD STATIC_AF_SDIO_CMD
#define STATIC_AF_SDCARD_D0 STATIC_AF_SDIO_D0
#define STATIC_AF_SDCARD_D1 STATIC_AF_SDIO_D1
#define STATIC_AF_SDCARD_D2 STATIC_AF_SDIO_D2
#define STATIC_AF_SDCARD_D3 STATIC_AF_SDIO_D3
#define STATIC_AF_SDCARD_D4 STATIC_AF_SDIO_D4
#define STATIC_AF_SDCARD_D5 STATIC_AF_SDIO_D5
#define STATIC_AF_SDCARD_D6 STATIC_AF_SDIO_D6
#define STATIC_AF_SDCARD_D7 STATIC_AF_SDIO_D7

#endif

// If no custom SDIO pins defined, use the default ones
#ifndef MICROPY_HW_SDCARD_CK
#define MICROPY_HW_SDCARD_D0 (pin_C8)
#define MICROPY_HW_SDCARD_D1 (pin_C9)
#define MICROPY_HW_SDCARD_D2 (pin_C10)
#define MICROPY_HW_SDCARD_D3 (pin_C11)
#define MICROPY_HW_SDCARD_CK (pin_C12)
#define MICROPY_HW_SDCARD_CMD (pin_D2)
#endif

// Define a constant to select the bus width.
#if MICROPY_HW_SDCARD_BUS_WIDTH == 4
#define SDIO_BUS_WIDE_VALUE SDIO_BUS_WIDE_4B
#elif MICROPY_HW_SDCARD_BUS_WIDTH == 8
#define SDIO_BUS_WIDE_VALUE SDIO_BUS_WIDE_8B
#endif

// A board can support UHS-I mode (1.8V signalling) on UHS-I capable SD cards by defining:
//   MICROPY_HW_SDCARD_VSELECT_PIN - output pin that switches the SD bus I/O rail (and the
//     corresponding MCU I/O voltage domain) between 3.3V (low) and 1.8V (high)
//   MICROPY_HW_SDCARD_RESET_PIN - active-low output pin controlling SD card VDD; it is also
//     sampled at startup to detect whether the hardware supports UHS-I (it reads high if so)
// and optionally:
//   MICROPY_HW_SDCARD_UHS_SWITCH_PATTERN - CMD6 pattern selecting the UHS-I speed mode
//   MICROPY_HW_SDCARD_UHS_CLK_DIV - SDMMC clock divider to use after the speed switch
#if defined(MICROPY_HW_SDCARD_VSELECT_PIN) && defined(MICROPY_HW_SDCARD_RESET_PIN)
#define SDCARD_ENABLE_UHS1 (1)
#if !defined(USE_SD_TRANSCEIVER) || USE_SD_TRANSCEIVER == 0
#error "UHS-I support requires USE_SD_TRANSCEIVER=1 in the HAL configuration"
#endif
#ifndef MICROPY_HW_SDCARD_UHS_SWITCH_PATTERN
#define MICROPY_HW_SDCARD_UHS_SWITCH_PATTERN SDMMC_SDR50_SWITCH_PATTERN
#endif
#ifndef MICROPY_HW_SDCARD_UHS_CLK_DIV
#define MICROPY_HW_SDCARD_UHS_CLK_DIV (1)
#endif
#if MICROPY_HW_SDCARD_SDMMC == 2
#define SDCARD_DLYB DLYB_SDMMC2
#else
#define SDCARD_DLYB DLYB_SDMMC1
#endif
#if defined(STM32N6)
// The SD bus pins sit in a switchable VDDIO power domain; a board can select which
// one with MICROPY_HW_SDCARD_VDDIO (2-5), defaulting to the domain that the used
// SDMMC's default pins are in.  The matching HSLV_VDDIOx OTP fuse must be set for
// the 1.8V pad voltage range to be selectable.
#ifndef MICROPY_HW_SDCARD_VDDIO
#if MICROPY_HW_SDCARD_SDMMC == 2
#define MICROPY_HW_SDCARD_VDDIO (5)
#else
#define MICROPY_HW_SDCARD_VDDIO (4)
#endif
#endif
#if MICROPY_HW_SDCARD_VDDIO == 2
#define SDCARD_VDDIO_SET_RANGE LL_PWR_SetVddIO2VoltageRange
#define SDCARD_VDDIO_GET_RANGE LL_PWR_GetVddIO2VoltageRange
#elif MICROPY_HW_SDCARD_VDDIO == 3
#define SDCARD_VDDIO_SET_RANGE LL_PWR_SetVddIO3VoltageRange
#define SDCARD_VDDIO_GET_RANGE LL_PWR_GetVddIO3VoltageRange
#elif MICROPY_HW_SDCARD_VDDIO == 4
#define SDCARD_VDDIO_SET_RANGE LL_PWR_SetVddIO4VoltageRange
#define SDCARD_VDDIO_GET_RANGE LL_PWR_GetVddIO4VoltageRange
#elif MICROPY_HW_SDCARD_VDDIO == 5
#define SDCARD_VDDIO_SET_RANGE LL_PWR_SetVddIO5VoltageRange
#define SDCARD_VDDIO_GET_RANGE LL_PWR_GetVddIO5VoltageRange
#else
#error "Invalid MICROPY_HW_SDCARD_VDDIO"
#endif
#endif
#else
#define SDCARD_ENABLE_UHS1 (0)
#endif

#define PYB_SDMMC_FLAG_SD       (0x01)
#define PYB_SDMMC_FLAG_MMC      (0x02)
#define PYB_SDMMC_FLAG_ACTIVE   (0x04)

static uint8_t pyb_sdmmc_flags;

#if SDCARD_ENABLE_UHS1
// Set if the hardware supports UHS-I, detected at startup via MICROPY_HW_SDCARD_RESET_PIN.
static bool sdcard_uhs_capable;
#endif

#define TIMEOUT_MS 30000

// TODO: I think that as an optimization, we can allocate these dynamically
//       if an sd card is detected. This will save approx 260 bytes of RAM
//       when no sdcard was being used.
static union {
    SD_HandleTypeDef sd;
    #if MICROPY_HW_ENABLE_MMCARD
    MMC_HandleTypeDef mmc;
    #endif
} sdmmc_handle;

static void sdcard_pin_config(void) {
    // configure SD GPIO
    // we do this here an not in HAL_SD_MspInit because it apparently
    // makes it more robust to have the pins always pulled high
    // Note: the mp_hal_pin_config function will configure the GPIO in
    // fast mode which can do up to 50MHz.  This should be plenty for SDIO
    // which clocks up to 25MHz maximum.
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_CK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_CK);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_CMD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_CMD);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D0);
    #if MICROPY_HW_SDCARD_BUS_WIDTH >= 4
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D1);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D2);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D3);
    #if MICROPY_HW_SDCARD_BUS_WIDTH == 8
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D4);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D5);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D6);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDCARD_D7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDCARD_D7);
    #endif
    #endif

    #if SDCARD_ENABLE_UHS1
    if (sdcard_uhs_capable) {
        // UHS-I bus rates need the fastest GPIO drive setting.
        mp_hal_pin_config_speed(MICROPY_HW_SDCARD_CK, MP_HAL_PIN_SPEED_VERY_HIGH);
        mp_hal_pin_config_speed(MICROPY_HW_SDCARD_CMD, MP_HAL_PIN_SPEED_VERY_HIGH);
        mp_hal_pin_config_speed(MICROPY_HW_SDCARD_D0, MP_HAL_PIN_SPEED_VERY_HIGH);
        #if MICROPY_HW_SDCARD_BUS_WIDTH >= 4
        mp_hal_pin_config_speed(MICROPY_HW_SDCARD_D1, MP_HAL_PIN_SPEED_VERY_HIGH);
        mp_hal_pin_config_speed(MICROPY_HW_SDCARD_D2, MP_HAL_PIN_SPEED_VERY_HIGH);
        mp_hal_pin_config_speed(MICROPY_HW_SDCARD_D3, MP_HAL_PIN_SPEED_VERY_HIGH);
        #endif
    }
    #endif
}

void sdcard_init(void) {
    // Set SD/MMC to no mode and inactive
    pyb_sdmmc_flags = 0;

    #if SDCARD_ENABLE_UHS1
    // Sample the SD reset pin to detect UHS-I capable hardware: it reads high when
    // the UHS-I power switch circuitry is present.
    mp_hal_pin_config(MICROPY_HW_SDCARD_RESET_PIN, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    sdcard_uhs_capable = mp_hal_pin_read(MICROPY_HW_SDCARD_RESET_PIN);
    if (sdcard_uhs_capable) {
        // Take over the card's power enable (keeping it powered) and select 3.3V signalling.
        mp_hal_pin_high(MICROPY_HW_SDCARD_RESET_PIN);
        mp_hal_pin_config(MICROPY_HW_SDCARD_RESET_PIN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
        mp_hal_pin_low(MICROPY_HW_SDCARD_VSELECT_PIN);
        mp_hal_pin_config(MICROPY_HW_SDCARD_VSELECT_PIN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
    }
    #endif

    sdcard_pin_config();

    // configure the SD card detect pin
    // we do this here so we can detect if the SD card is inserted before powering it on
    #if defined(MICROPY_HW_SDCARD_DETECT_PIN)
    mp_hal_pin_config(MICROPY_HW_SDCARD_DETECT_PIN, MP_HAL_PIN_MODE_INPUT, MICROPY_HW_SDCARD_DETECT_PULL, 0);
    #endif
}

void sdcard_select_sd(void) {
    pyb_sdmmc_flags |= PYB_SDMMC_FLAG_SD;
}

void sdcard_select_mmc(void) {
    pyb_sdmmc_flags |= PYB_SDMMC_FLAG_MMC;
}

static void sdmmc_msp_init(void) {
    // enable SDIO clock
    SDMMC_CLK_ENABLE();

    #if defined(STM32H7) || defined(STM32N6)
    // Reset SDMMC
    SDMMC_FORCE_RESET();
    SDMMC_RELEASE_RESET();
    #endif

    // NVIC configuration for SDIO interrupts
    NVIC_SetPriority(SDMMC_IRQn, IRQ_PRI_SDIO);
    HAL_NVIC_EnableIRQ(SDMMC_IRQn);

    // GPIO have already been initialised by sdcard_init
}

void sdmmc_msp_deinit(void) {
    HAL_NVIC_DisableIRQ(SDMMC_IRQn);
    SDMMC_CLK_DISABLE();
}

#if MICROPY_HW_ENABLE_SDCARD
void HAL_SD_MspInit(SD_HandleTypeDef *hsd) {
    sdmmc_msp_init();
}

void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd) {
    sdmmc_msp_deinit();
}
#endif

#if MICROPY_HW_ENABLE_MMCARD
void HAL_MMC_MspInit(MMC_HandleTypeDef *hsd) {
    sdmmc_msp_init();
}

void HAL_MMC_MspDeInit(MMC_HandleTypeDef *hsd) {
    sdmmc_msp_deinit();
}
#endif

bool sdcard_is_present(void) {
    #if MICROPY_HW_ENABLE_MMCARD
    if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
        return false;
    }
    #endif
    #if defined(MICROPY_HW_SDCARD_DETECT_PIN)
    return mp_hal_pin_read(MICROPY_HW_SDCARD_DETECT_PIN) == MICROPY_HW_SDCARD_DETECT_PRESENT;
    #else
    return true;
    #endif
}

#if MICROPY_HW_ENABLE_SDCARD

#if SDCARD_ENABLE_UHS1

// Switch the SD bus I/O voltage.  Called by the HAL during the CMD11 voltage switch
// sequence (with the bus clock stopped), and by this driver to restore 3.3V signalling.
void HAL_SD_DriveTransceiver_1_8V_Callback(FlagStatus status) {
    if (status == SET) {
        // The card acknowledged the switch: change the bus I/O rail to 1.8V and then
        // update the MCU pad voltage range to match the new rail voltage.
        mp_hal_pin_high(MICROPY_HW_SDCARD_VSELECT_PIN);
        mp_hal_delay_ms(1);
        #if defined(STM32N6)
        SDCARD_VDDIO_SET_RANGE(LL_PWR_VDDIO_VOLTAGE_RANGE_1V8);
        #endif
    } else {
        // Restore the pad voltage range first (a 1.8V pad range on a 3.3V rail can
        // damage the device), then switch the bus I/O rail back to 3.3V.
        #if defined(STM32N6)
        SDCARD_VDDIO_SET_RANGE(LL_PWR_VDDIO_VOLTAGE_RANGE_3V3);
        #endif
        mp_hal_pin_low(MICROPY_HW_SDCARD_VSELECT_PIN);
    }
}

static void sdcard_power_cycle(void) {
    // Return the bus to 3.3V signalling.
    HAL_SD_DriveTransceiver_1_8V_Callback(RESET);

    // Drive the SD bus pins low so the card is not back-powered via its I/O pins
    // while its supply is off.
    static const mp_hal_pin_obj_t bus_pins[] = {
        MICROPY_HW_SDCARD_CK, MICROPY_HW_SDCARD_CMD, MICROPY_HW_SDCARD_D0,
        #if MICROPY_HW_SDCARD_BUS_WIDTH >= 4
        MICROPY_HW_SDCARD_D1, MICROPY_HW_SDCARD_D2, MICROPY_HW_SDCARD_D3,
        #endif
    };
    for (size_t i = 0; i < MP_ARRAY_SIZE(bus_pins); ++i) {
        mp_hal_pin_low(bus_pins[i]);
        mp_hal_pin_config(bus_pins[i], MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
    }

    // Power cycle the SD card: its supply must fall below 0.5V for at least 1ms.
    mp_hal_pin_low(MICROPY_HW_SDCARD_RESET_PIN);
    mp_hal_delay_ms(20);
    mp_hal_pin_high(MICROPY_HW_SDCARD_RESET_PIN);
    mp_hal_delay_ms(10);

    // Restore the SD bus pins.
    sdcard_pin_config();
}

// Perform a CMD6 SWITCH_FUNC transaction, reading back the 64-byte switch status.
// The HAL's own CMD6 helpers arm the data path via DCTRL.DTEN without linking it to
// the command (CMDTRANS), which hangs on this SDMMC peripheral, so the transfer is
// done here using the same command-transfer mechanism as regular block reads.
static uint32_t sdcard_cmd6_switch(uint32_t arg, uint32_t *status) {
    SDMMC_TypeDef *sdmmc = sdmmc_handle.sd.Instance;

    uint32_t errorstate = SDMMC_CmdBlockLength(sdmmc, 64);
    if (errorstate != HAL_SD_ERROR_NONE) {
        return errorstate;
    }

    SDMMC_DataInitTypeDef config = {
        .DataTimeOut = 6250000, // 250ms at the 25MHz switching clock
        .DataLength = 64,
        .DataBlockSize = SDMMC_DATABLOCK_SIZE_64B,
        .TransferDir = SDMMC_TRANSFER_DIR_TO_SDMMC,
        .TransferMode = SDMMC_TRANSFER_MODE_BLOCK,
        .DPSM = SDMMC_DPSM_DISABLE,
    };
    (void)SDMMC_ConfigData(sdmmc, &config);
    __SDMMC_CMDTRANS_ENABLE(sdmmc);

    errorstate = SDMMC_CmdSwitch(sdmmc, arg);

    if (errorstate == HAL_SD_ERROR_NONE) {
        // The 64-byte status block fits entirely in the SDMMC FIFO, and takes well
        // under a millisecond to arrive at the 25MHz switching clock, so wait out
        // the transfer and then drain the FIFO unconditionally.  The status flags
        // cannot be used to pace the drain: after the 1.8V switch the data path
        // status reads stale until the FIFO is accessed, so the flag-driven receive
        // loops that the HAL uses deadlock here.
        mp_hal_delay_ms(2);
        for (int i = 0; i < 16; ++i) {
            status[i] = SDMMC_ReadFIFO(sdmmc);
        }
        // The status refreshes some time after the FIFO accesses above, so give it
        // a grace period before believing what it says.
        uint32_t sta;
        for (int i = 0; ; ++i) {
            sta = sdmmc->STA;
            if (sta & (SDMMC_FLAG_DATAEND | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DCRCFAIL |
                       SDMMC_FLAG_RXOVERR)) {
                break;
            }
            if (i >= 100) {
                break;
            }
            mp_hal_delay_us(100);
        }
        if (sta & (SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_RXOVERR)) {
            errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
        } else if (!(sta & SDMMC_FLAG_DATAEND)) {
            // The card did not send the status block.
            sdmmc->DCTRL = 0;
            errorstate = SDMMC_ERROR_TIMEOUT;
        }
    }

    __SDMMC_CMDTRANS_DISABLE(sdmmc);
    __SDMMC_CLEAR_FLAG(sdmmc, SDMMC_STATIC_DATA_FLAGS);

    return errorstate;
}

#endif // SDCARD_ENABLE_UHS1

static HAL_StatusTypeDef sdmmc_init_sd_bus(bool uhs) {
    (void)uhs;

    // SD device interface configuration
    sdmmc_handle.sd.Instance = SDIO;
    sdmmc_handle.sd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    #if !defined(STM32H5) && !defined(STM32H7) && !defined(STM32N6)
    sdmmc_handle.sd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    #endif
    sdmmc_handle.sd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_ENABLE;
    sdmmc_handle.sd.Init.BusWide = SDIO_BUS_WIDE_1B;
    sdmmc_handle.sd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    sdmmc_handle.sd.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;
    #if SDCARD_ENABLE_UHS1
    if (uhs) {
        // Run card identification and the 1.8V voltage switch at 25MHz; the clock is
        // raised to the UHS-I rate after the speed switch below.
        sdmmc_handle.sd.Init.TranceiverPresent = SDMMC_TRANSCEIVER_PRESENT;
        sdmmc_handle.sd.Init.ClockDiv = SDMMC_NSPEED_CLK_DIV;
        // Keep the bus clock free-running in UHS-I mode (matching the ST BSPs); with
        // clock power saving enabled the voltage switch and CMD6 sequences stall.
        sdmmc_handle.sd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    } else {
        sdmmc_handle.sd.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
    }
    #endif

    // init the SD interface, with retry if it's not ready yet
    HAL_StatusTypeDef status;
    for (int retry = 10; (status = HAL_SD_Init(&sdmmc_handle.sd)) != HAL_OK; retry--) {
        #if SDCARD_ENABLE_UHS1
        if (uhs) {
            // Don't retry a failed UHS-I init here: the card may be stuck mid voltage
            // switch and needs a power cycle, which the caller takes care of.
            return status;
        }
        #endif
        if (retry == 0) {
            return status;
        }
        mp_hal_delay_ms(50);
    }

    #if MICROPY_HW_SDCARD_BUS_WIDTH >= 4
    // configure the SD bus width for 4/8-bit wide operation
    status = HAL_SD_ConfigWideBusOperation(&sdmmc_handle.sd, SDIO_BUS_WIDE_VALUE);
    if (status != HAL_OK) {
        HAL_SD_DeInit(&sdmmc_handle.sd);
        return status;
    }
    #endif

    #if SDCARD_ENABLE_UHS1
    if (uhs) {
        bool uhs_active = sdmmc_handle.sd.SdCard.CardSpeed == CARD_ULTRA_HIGH_SPEED;
        #if defined(STM32N6)
        if (uhs_active) {
            // Verify that the pad voltage range switch took effect: without the
            // matching HSLV_VDDIOx OTP fuse the VRSEL write is silently ignored and
            // the pads remain in 3.3V mode, which passes the (slow) initialisation
            // but fails at UHS-I bus rates.  Fail the attempt here so the caller
            // power cycles the card and falls back to 3.3V operation.
            if (SDCARD_VDDIO_GET_RANGE() != LL_PWR_VDDIO_VOLTAGE_RANGE_1V8) {
                HAL_SD_DeInit(&sdmmc_handle.sd);
                return HAL_ERROR;
            }
        }
        #endif
        uint32_t pattern;
        uint32_t clk_div;
        if (uhs_active) {
            // The card accepted the 1.8V voltage switch during init: select the UHS-I
            // speed mode.
            pattern = MICROPY_HW_SDCARD_UHS_SWITCH_PATTERN;
            clk_div = MICROPY_HW_SDCARD_UHS_CLK_DIV;
        } else {
            // The card remained at 3.3V signalling: select high speed mode (SDR25).
            pattern = SDMMC_SDR25_SWITCH_PATTERN;
            clk_div = SDIO_TRANSFER_CLK_DIV;
        }
        uint32_t switch_status[16];
        uint32_t errorstate = sdcard_cmd6_switch(pattern, switch_status);
        if (errorstate == HAL_SD_ERROR_NONE
            && (((uint8_t *)switch_status)[16] & 0xf) != (pattern & 0xf)) {
            // The card did not switch to the requested speed mode.
            errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
        }
        if (errorstate != HAL_SD_ERROR_NONE) {
            HAL_SD_DeInit(&sdmmc_handle.sd);
            return HAL_ERROR;
        }
        if (uhs_active) {
            // Sample the incoming data on the feedback clock, via the delay block.
            MODIFY_REG(SDIO->CLKCR, SDMMC_CLKCR_SELCLKRX, SDMMC_CLKCR_SELCLKRX_1);
            LL_DLYB_Enable(SDCARD_DLYB);
            SDIO->CLKCR |= SDMMC_CLKCR_BUSSPEED;
        }
        // Raise the bus clock to its final rate.
        sdmmc_handle.sd.Init.ClockDiv = clk_div;
        MODIFY_REG(SDIO->CLKCR, SDMMC_CLKCR_CLKDIV, clk_div);
    }
    #endif

    return HAL_OK;
}

static HAL_StatusTypeDef sdmmc_init_sd(void) {
    #if SDCARD_ENABLE_UHS1
    if (sdcard_uhs_capable) {
        // Try to bring the card up in UHS-I mode.  Power cycle the card before each
        // attempt so it is in a known 3.3V state: a warm reset can leave the card in
        // 1.8V mode (in which case it will not advertise 1.8V support), and a failed
        // attempt can leave it stuck mid voltage switch.
        for (int attempt = 0; attempt < 2; ++attempt) {
            sdcard_power_cycle();
            HAL_StatusTypeDef status = sdmmc_init_sd_bus(true);
            if (status == HAL_OK) {
                return HAL_OK;
            }
        }
        // Fall back to a plain 3.3V init.
        sdcard_power_cycle();
    }
    #endif
    return sdmmc_init_sd_bus(false);
}
#endif

#if MICROPY_HW_ENABLE_MMCARD
static HAL_StatusTypeDef sdmmc_init_mmc(void) {
    // MMC device interface configuration
    sdmmc_handle.mmc.Instance = SDIO;
    sdmmc_handle.mmc.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    #ifndef STM32H7
    sdmmc_handle.mmc.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    #endif
    sdmmc_handle.mmc.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_ENABLE;
    sdmmc_handle.mmc.Init.BusWide = SDIO_BUS_WIDE_1B;
    sdmmc_handle.mmc.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    sdmmc_handle.mmc.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;

    // Init the SDIO interface
    HAL_StatusTypeDef status = HAL_MMC_Init(&sdmmc_handle.mmc);
    if (status != HAL_OK) {
        return status;
    }

    #ifdef MICROPY_HW_MMCARD_LOG_BLOCK_NBR
    // A board can override the number of logical blocks (card capacity) if needed.
    // This is needed when a card is high capacity because the extended CSD command
    // is not supported by the current version of the HAL.
    sdmmc_handle.mmc.MmcCard.LogBlockNbr = MICROPY_HW_MMCARD_LOG_BLOCK_NBR;
    #endif

    #if MICROPY_HW_SDCARD_BUS_WIDTH >= 4
    // Configure the SDIO bus width for 4/8-bit wide operation
    #ifdef STM32F7
    sdmmc_handle.mmc.Init.ClockBypass = SDIO_CLOCK_BYPASS_ENABLE;
    #endif
    status = HAL_MMC_ConfigWideBusOperation(&sdmmc_handle.mmc, SDIO_BUS_WIDE_VALUE);
    if (status != HAL_OK) {
        HAL_MMC_DeInit(&sdmmc_handle.mmc);
        return status;
    }
    #endif

    return HAL_OK;
}
#endif

bool sdcard_power_on(void) {
    if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_ACTIVE) {
        return true;
    }

    #ifdef MICROPY_BOARD_SDCARD_POWER
    MICROPY_BOARD_SDCARD_POWER
    #endif

    HAL_StatusTypeDef status = HAL_ERROR;
    switch (pyb_sdmmc_flags) {
        #if MICROPY_HW_ENABLE_SDCARD
        case PYB_SDMMC_FLAG_SD:
            if (sdcard_is_present()) {
                status = sdmmc_init_sd();
            }
            break;
        #endif
        #if MICROPY_HW_ENABLE_MMCARD
        case PYB_SDMMC_FLAG_MMC:
            status = sdmmc_init_mmc();
            break;
        #endif
    }

    if (status == HAL_OK) {
        pyb_sdmmc_flags |= PYB_SDMMC_FLAG_ACTIVE;
        return true;
    } else {
        return false;
    }
}

void sdcard_power_off(void) {
    switch (pyb_sdmmc_flags) {
        #if MICROPY_HW_ENABLE_SDCARD
        case PYB_SDMMC_FLAG_ACTIVE | PYB_SDMMC_FLAG_SD:
            HAL_SD_DeInit(&sdmmc_handle.sd);
            #if SDCARD_ENABLE_UHS1
            if (sdcard_uhs_capable) {
                // The card may be in 1.8V signalling mode; power cycle it so the next
                // init starts from a known 3.3V state.
                sdcard_power_cycle();
            }
            #endif
            break;
        #endif
        #if MICROPY_HW_ENABLE_MMCARD
        case PYB_SDMMC_FLAG_ACTIVE | PYB_SDMMC_FLAG_MMC:
            HAL_MMC_DeInit(&sdmmc_handle.mmc);
            break;
        #endif
    }
    pyb_sdmmc_flags &= ~PYB_SDMMC_FLAG_ACTIVE;
}

uint64_t sdcard_get_capacity_in_bytes(void) {
    switch (pyb_sdmmc_flags) {
        #if MICROPY_HW_ENABLE_SDCARD
        case PYB_SDMMC_FLAG_ACTIVE | PYB_SDMMC_FLAG_SD: {
            HAL_SD_CardInfoTypeDef cardinfo;
            HAL_SD_GetCardInfo(&sdmmc_handle.sd, &cardinfo);
            return (uint64_t)cardinfo.LogBlockNbr * (uint64_t)cardinfo.LogBlockSize;
        }
        #endif
        #if MICROPY_HW_ENABLE_MMCARD
        case PYB_SDMMC_FLAG_ACTIVE | PYB_SDMMC_FLAG_MMC: {
            HAL_MMC_CardInfoTypeDef cardinfo;
            HAL_MMC_GetCardInfo(&sdmmc_handle.mmc, &cardinfo);
            return (uint64_t)cardinfo.LogBlockNbr * (uint64_t)cardinfo.LogBlockSize;
        }
        #endif
        default:
            return 0;
    }
}

static void sdmmc_irq_handler(void) {
    switch (pyb_sdmmc_flags) {
        #if MICROPY_HW_ENABLE_SDCARD
        case PYB_SDMMC_FLAG_ACTIVE | PYB_SDMMC_FLAG_SD:
            HAL_SD_IRQHandler(&sdmmc_handle.sd);
            break;
        #endif
        #if MICROPY_HW_ENABLE_MMCARD
        case PYB_SDMMC_FLAG_ACTIVE | PYB_SDMMC_FLAG_MMC:
            HAL_MMC_IRQHandler(&sdmmc_handle.mmc);
            break;
        #endif
    }
}

void SDMMC_IRQHandler(void) {
    IRQ_ENTER(SDMMC_IRQn);
    sdmmc_irq_handler();
    IRQ_EXIT(SDMMC_IRQn);
}

static void sdcard_reset_periph(void) {
    // Fully reset the SDMMC peripheral before calling HAL SD DMA functions.
    // (There could be an outstanding DTIMEOUT event from a previous call and the
    // HAL function enables IRQs before fully configuring the SDMMC peripheral.)
    SDIO->DTIMER = 0;
    SDIO->DLEN = 0;
    SDIO->DCTRL = 0;
    SDIO->ICR = SDMMC_STATIC_FLAGS;
}

static HAL_StatusTypeDef sdcard_wait_finished(void) {
    // Wait for HAL driver to be ready (eg for DMA to finish)
    uint32_t start = HAL_GetTick();
    for (;;) {
        // Do an atomic check of the state; WFI will exit even if IRQs are disabled
        uint32_t irq_state = disable_irq();
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            if (sdmmc_handle.mmc.State != HAL_MMC_STATE_BUSY) {
                enable_irq(irq_state);
                break;
            }
        } else
        #endif
        {
            if (sdmmc_handle.sd.State != HAL_SD_STATE_BUSY) {
                enable_irq(irq_state);
                break;
            }
        }
        __WFI();
        enable_irq(irq_state);
        if (HAL_GetTick() - start >= TIMEOUT_MS) {
            return HAL_TIMEOUT;
        }
    }

    // Wait for SD card to complete the operation
    for (;;) {
        uint32_t state;
        #if MICROPY_HW_ENABLE_MMCARD
        MP_STATIC_ASSERT((uint32_t)HAL_SD_CARD_TRANSFER == (uint32_t)HAL_MMC_CARD_TRANSFER);
        MP_STATIC_ASSERT((uint32_t)HAL_SD_CARD_SENDING == (uint32_t)HAL_MMC_CARD_SENDING);
        MP_STATIC_ASSERT((uint32_t)HAL_SD_CARD_RECEIVING == (uint32_t)HAL_MMC_CARD_RECEIVING);
        MP_STATIC_ASSERT((uint32_t)HAL_SD_CARD_PROGRAMMING == (uint32_t)HAL_MMC_CARD_PROGRAMMING);
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            state = HAL_MMC_GetCardState(&sdmmc_handle.mmc);
        } else
        #endif
        {
            state = HAL_SD_GetCardState(&sdmmc_handle.sd);
        }

        if (state == HAL_SD_CARD_TRANSFER) {
            return HAL_OK;
        }
        if (!(state == HAL_SD_CARD_SENDING || state == HAL_SD_CARD_RECEIVING || state == HAL_SD_CARD_PROGRAMMING)) {
            return HAL_ERROR;
        }
        if (HAL_GetTick() - start >= TIMEOUT_MS) {
            return HAL_TIMEOUT;
        }
        __WFI();
    }
    return HAL_OK;
}

static int sdcard_common_checks(uint32_t block_num, uint32_t num_blocks) {
    // check that SD card is initialised
    if (!(pyb_sdmmc_flags & PYB_SDMMC_FLAG_ACTIVE)) {
        return -MP_EIO;
    }

    // check that adding block_num & num_blocks don't overflow
    // (the ST HAL does a bounds check, but only after adding them...)
    uint32_t end_block;
    if (__builtin_add_overflow(block_num, num_blocks, &end_block)) {
        return -MP_EIO;
    }

    return 0;
}

int sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    int ret = sdcard_common_checks(block_num, num_blocks);
    if (ret != 0) {
        return ret;
    }

    // check that dest pointer is aligned on a 4-byte boundary
    uint8_t *orig_dest = NULL;
    uint32_t saved_word;
    if (((uint32_t)dest & 3) != 0) {
        // Pointer is not aligned so it needs fixing.
        // We could allocate a temporary block of RAM (as sdcard_write_blocks
        // does) but instead we are going to use the dest buffer inplace.  We
        // are going to align the pointer, save the initial word at the aligned
        // location, read into the aligned memory, move the memory back to the
        // unaligned location, then restore the initial bytes at the aligned
        // location.  We should have no trouble doing this as those initial
        // bytes at the aligned location should be able to be changed for the
        // duration of this function call.
        orig_dest = dest;
        dest = (uint8_t *)((uint32_t)dest & ~3);
        saved_word = *(uint32_t *)dest;
    }

    HAL_StatusTypeDef err = HAL_OK;

    if (query_irq() == IRQ_STATE_ENABLED) {
        #if MICROPY_HW_USB_MSC
        uint32_t basepri;
        bool usb_msc_sdcard = usbd_msc_lu_includes_sdcard();
        if (usb_msc_sdcard) {
            // we must disable USB irqs to prevent MSC contention with SD card
            basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
        }
        #endif

        #if SDIO_USE_GPDMA
        DMA_HandleTypeDef sd_dma;
        dma_init(&sd_dma, &SDMMC_DMA, DMA_PERIPH_TO_MEMORY, &sdmmc_handle);
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            sdmmc_handle.mmc.hdmarx = &sd_dma;
        } else
        #endif
        {
            sdmmc_handle.sd.hdmarx = &sd_dma;
        }
        #endif

        // make sure cache is flushed and invalidated so when DMA updates the RAM
        // from reading the peripheral the CPU then reads the new data
        dma_protect_rx_region(dest, num_blocks * SDCARD_BLOCK_SIZE);

        sdcard_reset_periph();
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            err = HAL_MMC_ReadBlocks_DMA(&sdmmc_handle.mmc, dest, block_num, num_blocks);
        } else
        #endif
        {
            err = HAL_SD_ReadBlocks_DMA(&sdmmc_handle.sd, dest, block_num, num_blocks);
        }
        if (err == HAL_OK) {
            err = sdcard_wait_finished();
        }

        dma_unprotect_rx_region(dest, num_blocks * SDCARD_BLOCK_SIZE);

        #if SDIO_USE_GPDMA
        dma_deinit(&SDMMC_DMA);
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            sdmmc_handle.mmc.hdmarx = NULL;
        } else
        #endif
        {
            sdmmc_handle.sd.hdmarx = NULL;
        }
        #endif

        #if MICROPY_HW_USB_MSC
        if (usb_msc_sdcard) {
            restore_irq_pri(basepri);
        }
        #endif
    } else {
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            err = HAL_MMC_ReadBlocks(&sdmmc_handle.mmc, dest, block_num, num_blocks, TIMEOUT_MS);
        } else
        #endif
        {
            err = HAL_SD_ReadBlocks(&sdmmc_handle.sd, dest, block_num, num_blocks, TIMEOUT_MS);
        }
        if (err == HAL_OK) {
            err = sdcard_wait_finished();
        }
    }

    if (orig_dest != NULL) {
        // move the read data to the non-aligned position, and restore the initial bytes
        memmove(orig_dest, dest, num_blocks * SDCARD_BLOCK_SIZE);
        memcpy(dest, &saved_word, orig_dest - dest);
    }

    return mp_hal_status_to_neg_errno(err);
}

int sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    int ret = sdcard_common_checks(block_num, num_blocks);
    if (ret != 0) {
        return ret;
    }

    // check that src pointer is aligned on a 4-byte boundary
    if (((uint32_t)src & 3) != 0) {
        // pointer is not aligned, so allocate a temporary block to do the write
        uint8_t *src_aligned = m_new_maybe(uint8_t, SDCARD_BLOCK_SIZE);
        if (src_aligned == NULL) {
            return -MP_EIO;
        }
        for (size_t i = 0; i < num_blocks; ++i) {
            memcpy(src_aligned, src + i * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
            ret = sdcard_write_blocks(src_aligned, block_num + i, 1);
            if (ret != 0) {
                break;
            }
        }
        m_del(uint8_t, src_aligned, SDCARD_BLOCK_SIZE);
        return ret;
    }

    HAL_StatusTypeDef err = HAL_OK;

    if (query_irq() == IRQ_STATE_ENABLED) {
        #if MICROPY_HW_USB_MSC
        uint32_t basepri;
        bool usb_msc_sdcard = usbd_msc_lu_includes_sdcard();
        if (usb_msc_sdcard) {
            // we must disable USB irqs to prevent MSC contention with SD card
            basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
        }
        #endif

        #if SDIO_USE_GPDMA
        DMA_HandleTypeDef sd_dma;
        dma_init(&sd_dma, &SDMMC_DMA, DMA_MEMORY_TO_PERIPH, &sdmmc_handle);
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            sdmmc_handle.mmc.hdmatx = &sd_dma;
        } else
        #endif
        {
            sdmmc_handle.sd.hdmatx = &sd_dma;
        }
        #endif

        // make sure cache is flushed to RAM so the DMA can read the correct data
        MP_HAL_CLEAN_DCACHE(src, num_blocks * SDCARD_BLOCK_SIZE);

        sdcard_reset_periph();
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            err = HAL_MMC_WriteBlocks_DMA(&sdmmc_handle.mmc, (uint8_t *)src, block_num, num_blocks);
        } else
        #endif
        {
            err = HAL_SD_WriteBlocks_DMA(&sdmmc_handle.sd, (uint8_t *)src, block_num, num_blocks);
        }
        if (err == HAL_OK) {
            err = sdcard_wait_finished();
        }

        #if SDIO_USE_GPDMA
        dma_deinit(&SDMMC_DMA);
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            sdmmc_handle.mmc.hdmatx = NULL;
        } else
        #endif
        {
            sdmmc_handle.sd.hdmatx = NULL;
        }
        #endif

        #if MICROPY_HW_USB_MSC
        if (usb_msc_sdcard) {
            restore_irq_pri(basepri);
        }
        #endif
    } else {
        #if MICROPY_HW_ENABLE_MMCARD
        if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
            err = HAL_MMC_WriteBlocks(&sdmmc_handle.mmc, (uint8_t *)src, block_num, num_blocks, TIMEOUT_MS);
        } else
        #endif
        {
            err = HAL_SD_WriteBlocks(&sdmmc_handle.sd, (uint8_t *)src, block_num, num_blocks, TIMEOUT_MS);
        }
        if (err == HAL_OK) {
            err = sdcard_wait_finished();
        }
    }

    return mp_hal_status_to_neg_errno(err);
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the SD card or MMC as an object with the block protocol.

#if !BUILDING_MBOOT

// There are singleton SDCard/MMCard objects
#if MICROPY_HW_ENABLE_SDCARD
const mp_obj_base_t pyb_sdcard_obj = {&machine_sdcard_type};
#endif
#if MICROPY_HW_ENABLE_MMCARD
const mp_obj_base_t pyb_mmcard_obj = {&pyb_mmcard_type};
#endif

#if MICROPY_HW_ENABLE_SDCARD
static mp_obj_t pyb_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    #if MICROPY_HW_ENABLE_MMCARD
    if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
        mp_raise_ValueError(MP_ERROR_TEXT("peripheral used by MMCard"));
    }
    #endif

    pyb_sdmmc_flags |= PYB_SDMMC_FLAG_SD;

    // return singleton object
    return MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
}
#endif

#if MICROPY_HW_ENABLE_MMCARD
static mp_obj_t pyb_mmcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    #if MICROPY_HW_ENABLE_SDCARD
    if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_SD) {
        mp_raise_ValueError(MP_ERROR_TEXT("peripheral used by SDCard"));
    }
    #endif

    pyb_sdmmc_flags |= PYB_SDMMC_FLAG_MMC;

    // return singleton object
    return MP_OBJ_FROM_PTR(&pyb_mmcard_obj);
}
#endif

static mp_obj_t sd_present(mp_obj_t self) {
    return mp_obj_new_bool(sdcard_is_present());
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_present_obj, sd_present);

static mp_obj_t sd_power(mp_obj_t self, mp_obj_t state) {
    bool result;
    if (mp_obj_is_true(state)) {
        result = sdcard_power_on();
    } else {
        sdcard_power_off();
        result = true;
    }
    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_power_obj, sd_power);

static mp_obj_t sd_info(mp_obj_t self) {
    if (!(pyb_sdmmc_flags & PYB_SDMMC_FLAG_ACTIVE)) {
        return mp_const_none;
    }
    uint32_t card_type;
    uint32_t log_block_nbr;
    uint32_t log_block_size;
    #if MICROPY_HW_ENABLE_MMCARD
    if (pyb_sdmmc_flags & PYB_SDMMC_FLAG_MMC) {
        card_type = sdmmc_handle.mmc.MmcCard.CardType;
        log_block_nbr = sdmmc_handle.mmc.MmcCard.LogBlockNbr;
        log_block_size = sdmmc_handle.mmc.MmcCard.LogBlockSize;
    } else
    #endif
    {
        card_type = sdmmc_handle.sd.SdCard.CardType;
        log_block_nbr = sdmmc_handle.sd.SdCard.LogBlockNbr;
        log_block_size = sdmmc_handle.sd.SdCard.LogBlockSize;
    }
    // cardinfo.SD_csd and cardinfo.SD_cid have lots of info but we don't use them
    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull((uint64_t)log_block_nbr * (uint64_t)log_block_size),
        mp_obj_new_int_from_uint(log_block_size),
        mp_obj_new_int(card_type),
    };
    return mp_obj_new_tuple(3, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);

// now obsolete, kept for backwards compatibility
static mp_obj_t sd_read(mp_obj_t self, mp_obj_t block_num) {
    uint8_t *dest = m_new(uint8_t, SDCARD_BLOCK_SIZE);
    int ret = sdcard_read_blocks(dest, mp_obj_get_int(block_num), 1);

    if (ret != 0) {
        m_del(uint8_t, dest, SDCARD_BLOCK_SIZE);
        mp_raise_msg_varg(&mp_type_Exception, MP_ERROR_TEXT("sdcard_read_blocks failed [%d]"), ret);
    }

    return mp_obj_new_bytearray_by_ref(SDCARD_BLOCK_SIZE, dest);
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_read_obj, sd_read);

// now obsolete, kept for backwards compatibility
static mp_obj_t sd_write(mp_obj_t self, mp_obj_t block_num, mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len % SDCARD_BLOCK_SIZE != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("writes must be a multiple of %d bytes"), SDCARD_BLOCK_SIZE);
    }

    int ret = sdcard_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_Exception, MP_ERROR_TEXT("sdcard_write_blocks failed [%d]"), ret);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(sd_write_obj, sd_write);

static mp_obj_t pyb_sdcard_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    int ret = sdcard_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_readblocks_obj, pyb_sdcard_readblocks);

static mp_obj_t pyb_sdcard_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    int ret = sdcard_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_writeblocks_obj, pyb_sdcard_writeblocks);

static mp_obj_t pyb_sdcard_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            if (!sdcard_power_on()) {
                return MP_OBJ_NEW_SMALL_INT(-1); // error
            }
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case MP_BLOCKDEV_IOCTL_DEINIT:
            sdcard_power_off();
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case MP_BLOCKDEV_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(sdcard_get_capacity_in_bytes() / SDCARD_BLOCK_SIZE);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SDCARD_BLOCK_SIZE);

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_ioctl_obj, pyb_sdcard_ioctl);

static const mp_rom_map_elem_t pyb_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_present), MP_ROM_PTR(&sd_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_power), MP_ROM_PTR(&sd_power_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&sd_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&sd_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&sd_write_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_sdcard_ioctl_obj) },
};

static MP_DEFINE_CONST_DICT(pyb_sdcard_locals_dict, pyb_sdcard_locals_dict_table);

#if MICROPY_HW_ENABLE_SDCARD
MP_DEFINE_CONST_OBJ_TYPE(
    machine_sdcard_type,
    MP_QSTR_SDCard,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_sdcard_make_new,
    locals_dict, &pyb_sdcard_locals_dict
    );
#endif

#if MICROPY_HW_ENABLE_MMCARD
MP_DEFINE_CONST_OBJ_TYPE(
    pyb_mmcard_type,
    MP_QSTR_MMCard,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_mmcard_make_new,
    locals_dict, &pyb_sdcard_locals_dict
    );
#endif

void sdcard_init_vfs(fs_user_mount_t *vfs, int part) {
    pyb_sdmmc_flags = (pyb_sdmmc_flags & PYB_SDMMC_FLAG_ACTIVE) | PYB_SDMMC_FLAG_SD; // force SD mode
    vfs->base.type = &mp_fat_vfs_type;
    vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_NATIVE | MP_BLOCKDEV_FLAG_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    #if MICROPY_FATFS_MULTI_PARTITION
    vfs->fatfs.part = part;
    #endif
    vfs->blockdev.readblocks[0] = MP_OBJ_FROM_PTR(&pyb_sdcard_readblocks_obj);
    vfs->blockdev.readblocks[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
    vfs->blockdev.readblocks[2] = MP_OBJ_FROM_PTR(sdcard_read_blocks); // native version
    vfs->blockdev.writeblocks[0] = MP_OBJ_FROM_PTR(&pyb_sdcard_writeblocks_obj);
    vfs->blockdev.writeblocks[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
    vfs->blockdev.writeblocks[2] = MP_OBJ_FROM_PTR(sdcard_write_blocks); // native version
    vfs->blockdev.u.ioctl[0] = MP_OBJ_FROM_PTR(&pyb_sdcard_ioctl_obj);
    vfs->blockdev.u.ioctl[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
}

#endif // !BUILDING_MBOOT

#endif // MICROPY_HW_ENABLE_SDCARD || MICROPY_HW_ENABLE_MMCARD
