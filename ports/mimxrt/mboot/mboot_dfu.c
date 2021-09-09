/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

// include fsl_clock.h BEFORE including clock configuration due to bug in NXP HAL.
#include "fsl_clock.h"
#include CLOCK_CONFIG_H
#include "mboot_dfu.h"
#include "mboot_memory.h"
#include "mboot_upgrade.h"
#include "mboot_utils.h"
#include "mboot_validate.h"
#include "tusb.h"
#include "tusb_config.h"

// --------------------------------------------------------------------+
// Constants
// --------------------------------------------------------------------+
const uint32_t dfu_finish_delay_time_ms = 500;

// --------------------------------------------------------------------+
// Local Variables
// --------------------------------------------------------------------+
static bool dfu_finished = false;
static int32_t bytes_to_ignore = 0UL;

// --------------------------------------------------------------------+
// External Variables
// --------------------------------------------------------------------+
extern uint32_t __flash_config_start;  // Linker symbol
extern uint32_t __firmware_start;  // Linker symbol

// --------------------------------------------------------------------+
// Local Function Declarations
// --------------------------------------------------------------------+

void usb_init(uint8_t d_cal, uint8_t txcal45dp, uint8_t txcal45dn);

// --------------------------------------------------------------------+
// Global Functions
// --------------------------------------------------------------------+

void mboot_dfu_init(void) {
    usb_init(0b0111, 0b0110, 0b0110);  // Configure nominal values for D_CAL and TXCAL45DP/DN
    (void)mboot_upgrade_init();
    tusb_init();
    // ---
    dfu_finished = false;
    bytes_to_ignore = (int32_t)((uint32_t)MEM_GET_SYMBOL_VALUE(__firmware_start)) - ((uint32_t)MEM_GET_SYMBOL_VALUE(__flash_config_start));
}

void mboot_dfu_run(void) {
    while (dfu_finished == false) {
        tud_task(); // tinyusb device task
    }
}

// Invoked right before tud_dfu_download_cb() (state=DFU_DNBUSY) or tud_dfu_manifest_cb() (state=DFU_MANIFEST)


// Invoked when received DFU_DNLOAD (wLength>0) following by DFU_GETSTATUS (state=DFU_DNBUSY) requests
void tud_dfu_download_cb(uint8_t alt, uint16_t block_num, uint8_t const *data, uint16_t length);

// Invoked when download process is complete, received DFU_DNLOAD (wLength=0) following by DFU_GETSTATUS (state=Manifest)


// Invoked when a DFU_DETACH request is received
TU_ATTR_WEAK void tud_dfu_detach_cb(void);

// --------------------------------------------------------------------+
// Device callbacks
// --------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
    return;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    (void)mboot_upgrade_deinit();
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wake-up
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    (void)mboot_upgrade_deinit();
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    (void)mboot_upgrade_deinit();
}

// --------------------------------------------------------------------+
// DFU callbacks
// --------------------------------------------------------------------+

uint32_t tud_dfu_get_timeout_cb(uint8_t alt, uint8_t state) {
    (void)alt;   // Todo: Implement handling of alternative option in the future
    return 0;
}

void tud_dfu_download_cb(uint8_t alt, uint16_t block_num, uint8_t const *data, uint16_t length) {
    (void)alt;   // Todo: Implement handling of alternative option in the future


    mboot_upgrade_status_t status = BU_STATUS_UNKNOWN_ERROR;

    if (bytes_to_ignore > length) {
        bytes_to_ignore -= length;
        status = BU_STATUS_OK;
    } else {
        length = length - bytes_to_ignore;
        status = mboot_upgrade_write_data(block_num, data + bytes_to_ignore, length);
        bytes_to_ignore = 0;
    }

    if (status == BU_STATUS_OK) {
        tud_dfu_finish_flashing(DFU_STATUS_OK);
    } else {
        mboot_upgrade_deinit();
        tud_dfu_finish_flashing(DFU_STATUS_ERR_PROG);
    }
}

void tud_dfu_manifest_cb(uint8_t alt) {
    (void)alt;

    dfu_status_t dfu_status = DFU_STATUS_ERR_UNKNOWN;
    mboot_upgrade_status_t status = BU_STATUS_UNKNOWN_ERROR;

    status = mboot_upgrade_finalize();

    if (status == BU_STATUS_OK) {
        status = mboot_upgrade_validate();
        if (status == BU_STATUS_OK) {
            dfu_status = DFU_STATUS_OK;
        } else {
            mboot_upgrade_deinit();
            dfu_status = DFU_STATUS_ERR_VERIFY;
        }
    } else {
        mboot_upgrade_deinit();
        dfu_status = DFU_STATUS_ERR_PROG;
    }

    tud_dfu_finish_flashing(dfu_status);
}

void tud_dfu_abort_cb(uint8_t alt) {
    (void)alt;   // Todo: Implement handling of alternative option in the future
    (void)mboot_upgrade_deinit();
}

void tud_dfu_detach_cb(void) {
    dfu_finished = true;
}

uint16_t tud_dfu_upload_cb(uint8_t alt, uint16_t block_num, uint8_t *data, uint16_t length) {
    (void)alt;
    (void)block_num;
    (void)data;
    (void)length;
    return 0UL;
}

// --------------------------------------------------------------------+
// Helper functions
// --------------------------------------------------------------------+
void usb_init(uint8_t d_cal, uint8_t txcal45dp, uint8_t txcal45dn) {
    #ifdef USBPHY1
    USBPHY_Type *usb_phy = USBPHY1;
    #else
    USBPHY_Type *usb_phy = USBPHY;
    #endif

    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, BOARD_XTAL0_CLK_HZ);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, BOARD_XTAL0_CLK_HZ);

    #if defined(CPU_MIMXRT1176_cm7)
    usb_phy->TRIM_OVERRIDE_EN = USBPHY_TRIM_OVERRIDE_EN_TRIM_DIV_SEL_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_ENV_TAIL_ADJ_VD_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_TX_D_CAL_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_TX_CAL45DP_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_TX_CAL45DN_OVERRIDE(1);                              // Enable override for D_CAL and TXCAL45DP/DN
    #endif
    usb_phy->PWD = 0U;  // Set all bits in PWD register to normal operation
    usb_phy->TX = ((usb_phy->TX & (~(USBPHY_TX_D_CAL_MASK | USBPHY_TX_TXCAL45DM_MASK | USBPHY_TX_TXCAL45DP_MASK))) |
        (USBPHY_TX_D_CAL(d_cal) | USBPHY_TX_TXCAL45DP(txcal45dp) | USBPHY_TX_TXCAL45DM(txcal45dn)));              // Configure values for D_CAL and TXCAL45DP/DN
}

void USB_OTG1_IRQHandler(void) {
    tud_int_handler(0);
    __SEV();
}
