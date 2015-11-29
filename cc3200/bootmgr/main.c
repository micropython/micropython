/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <stdbool.h>

#include "std.h"

#include "py/mpconfig.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_gpio.h"
#include "hw_memmap.h"
#include "hw_gprcm.h"
#include "hw_common_reg.h"
#include "pin.h"
#include "gpio.h"
#include "rom_map.h"
#include "prcm.h"
#include "simplelink.h"
#include "interrupt.h"
#include "gpio.h"
#include "flc.h"
#include "bootmgr.h"
#include "shamd5.h"
#include "cryptohash.h"
#include "utils.h"
#include "cc3200_hal.h"
#include "debug.h"
#include "mperror.h"
#include "antenna.h"


//*****************************************************************************
// Local Constants
//*****************************************************************************
#define SL_STOP_TIMEOUT                     35
#define BOOTMGR_HASH_ALGO                   SHAMD5_ALGO_MD5
#define BOOTMGR_HASH_SIZE                   32
#define BOOTMGR_BUFF_SIZE                   512

#define BOOTMGR_WAIT_SAFE_MODE_0_MS         500

#define BOOTMGR_WAIT_SAFE_MODE_1_MS         3000
#define BOOTMGR_WAIT_SAFE_MODE_1_BLINK_MS   500

#define BOOTMGR_WAIT_SAFE_MODE_2_MS         3000
#define BOOTMGR_WAIT_SAFE_MODE_2_BLINK_MS   250

#define BOOTMGR_WAIT_SAFE_MODE_3_MS         1500
#define BOOTMGR_WAIT_SAFE_MODE_3_BLINK_MS   100

//*****************************************************************************
// Exported functions declarations
//*****************************************************************************
extern void bootmgr_run_app (_u32 base);

//*****************************************************************************
// Local functions declarations
//*****************************************************************************
static void bootmgr_board_init (void);
static bool bootmgr_verify (_u8 *image);
static void bootmgr_load_and_execute (_u8 *image);
static bool wait_while_blinking (uint32_t wait_time, uint32_t period, bool force_wait);
static bool safe_boot_request_start (uint32_t wait_time);
static void wait_for_safe_boot (sBootInfo_t *psBootInfo);
static void bootmgr_image_loader (sBootInfo_t *psBootInfo);

//*****************************************************************************
// Private data
//*****************************************************************************
static _u8 bootmgr_file_buf[BOOTMGR_BUFF_SIZE];
static _u8 bootmgr_hash_buf[BOOTMGR_HASH_SIZE + 1];

//*****************************************************************************
// Vector Table
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);

//*****************************************************************************
// WLAN Event handler callback hookup function
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{

}

//*****************************************************************************
// HTTP Server callback hookup function
//*****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{

}

//*****************************************************************************
// Net APP Event callback hookup function
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{

}

//*****************************************************************************
// General Event callback hookup function
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{

}

//*****************************************************************************
// Socket Event callback hookup function
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{

}

//*****************************************************************************
//! Board Initialization & Configuration
//*****************************************************************************
static void bootmgr_board_init(void) {
    // set the vector table base
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);

    // enable processor interrupts
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    // mandatory MCU initialization
    PRCMCC3200MCUInit();

    // clear all the special bits, since we can't trust their content after reset
    // except for the WDT reset one!!
    PRCMClearSpecialBit(PRCM_SAFE_BOOT_BIT);
    PRCMClearSpecialBit(PRCM_FIRST_BOOT_BIT);

    // check the reset after clearing the special bits
    mperror_bootloader_check_reset_cause();

#if MICROPY_HW_ANTENNA_DIVERSITY
    // configure the antenna selection pins
    antenna_init0();
#endif

    // enable the data hashing engine
    CRYPTOHASH_Init();

    // init the system led and the system switch
    mperror_init0();
}

//*****************************************************************************
//! Verifies the integrity of the new application binary
//*****************************************************************************
static bool bootmgr_verify (_u8 *image) {
    SlFsFileInfo_t FsFileInfo;
    _u32 reqlen, offset = 0;
    _i32 fHandle;

    // open the file for reading
    if (0 == sl_FsOpen(image, FS_MODE_OPEN_READ, NULL, &fHandle)) {
        // get the file size
        sl_FsGetInfo(image, 0, &FsFileInfo);

        if (FsFileInfo.FileLen > BOOTMGR_HASH_SIZE) {
            FsFileInfo.FileLen -= BOOTMGR_HASH_SIZE;
            CRYPTOHASH_SHAMD5Start(BOOTMGR_HASH_ALGO, FsFileInfo.FileLen);
            do {
                if ((FsFileInfo.FileLen - offset) > BOOTMGR_BUFF_SIZE) {
                    reqlen = BOOTMGR_BUFF_SIZE;
                }
                else {
                    reqlen = FsFileInfo.FileLen - offset;
                }

                offset += sl_FsRead(fHandle, offset, bootmgr_file_buf, reqlen);
                CRYPTOHASH_SHAMD5Update(bootmgr_file_buf, reqlen);
            } while (offset < FsFileInfo.FileLen);

            CRYPTOHASH_SHAMD5Read (bootmgr_file_buf);

            // convert the resulting hash to hex
            for (_u32 i = 0; i < (BOOTMGR_HASH_SIZE / 2); i++) {
                snprintf ((char *)&bootmgr_hash_buf[(i * 2)], 3, "%02x", bootmgr_file_buf[i]);
            }

            // read the hash from the file and close it
            sl_FsRead(fHandle, offset, bootmgr_file_buf, BOOTMGR_HASH_SIZE);
            sl_FsClose (fHandle, NULL, NULL, 0);
            bootmgr_file_buf[BOOTMGR_HASH_SIZE] = '\0';
            // compare both hashes
            if (!strcmp((const char *)bootmgr_hash_buf, (const char *)bootmgr_file_buf)) {
                // it's a match
                return true;
            }
        }
        // close the file
        sl_FsClose(fHandle, NULL, NULL, 0);
    }
    return false;
}

//*****************************************************************************
//! Loads the application from sFlash and executes
//*****************************************************************************
static void bootmgr_load_and_execute (_u8 *image) {
    SlFsFileInfo_t pFsFileInfo;
    _i32 fhandle;
    // open the application binary
    if (!sl_FsOpen(image, FS_MODE_OPEN_READ, NULL, &fhandle)) {
        // get the file size
        if (!sl_FsGetInfo(image, 0, &pFsFileInfo)) {
            // read the application into SRAM
            if (pFsFileInfo.FileLen == sl_FsRead(fhandle, 0, (unsigned char *)APP_IMG_SRAM_OFFSET, pFsFileInfo.FileLen)) {
                // close the file
                sl_FsClose(fhandle, 0, 0, 0);
                // stop the network services
                sl_Stop(SL_STOP_TIMEOUT);
                // execute the application
                bootmgr_run_app(APP_IMG_SRAM_OFFSET);
            }
        }
    }
}

//*****************************************************************************
//! Wait while the safe mode pin is being held high and blink the system led
//! with the specified period
//*****************************************************************************
static bool wait_while_blinking (uint32_t wait_time, uint32_t period, bool force_wait) {
    _u32 count;
    for (count = 0; (force_wait || MAP_GPIOPinRead(MICROPY_SAFE_BOOT_PORT, MICROPY_SAFE_BOOT_PORT_PIN)) &&
         ((period * count) < wait_time); count++) {
        // toogle the led
        MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, ~MAP_GPIOPinRead(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN));
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(period * 1000));
    }
    return MAP_GPIOPinRead(MICROPY_SAFE_BOOT_PORT, MICROPY_SAFE_BOOT_PORT_PIN) ? true : false;
}

static bool safe_boot_request_start (uint32_t wait_time) {
    if (MAP_GPIOPinRead(MICROPY_SAFE_BOOT_PORT, MICROPY_SAFE_BOOT_PORT_PIN)) {
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(wait_time * 1000));
    }
    return MAP_GPIOPinRead(MICROPY_SAFE_BOOT_PORT, MICROPY_SAFE_BOOT_PORT_PIN) ? true : false;
}

//*****************************************************************************
//! Check for the safe mode pin
//*****************************************************************************
static void wait_for_safe_boot (sBootInfo_t *psBootInfo) {
    if (safe_boot_request_start(BOOTMGR_WAIT_SAFE_MODE_0_MS)) {
        if (wait_while_blinking(BOOTMGR_WAIT_SAFE_MODE_1_MS, BOOTMGR_WAIT_SAFE_MODE_1_BLINK_MS, false)) {
            // go back one step in time
            psBootInfo->ActiveImg = psBootInfo->PrevImg;
            if (wait_while_blinking(BOOTMGR_WAIT_SAFE_MODE_2_MS, BOOTMGR_WAIT_SAFE_MODE_2_BLINK_MS, false)) {
                // go back directly to the factory image
                psBootInfo->ActiveImg = IMG_ACT_FACTORY;
                wait_while_blinking(BOOTMGR_WAIT_SAFE_MODE_3_MS, BOOTMGR_WAIT_SAFE_MODE_3_BLINK_MS, true);
            }
        }
        // turn off the system led
        MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, 0);
        // request a safe boot to the application
        PRCMSetSpecialBit(PRCM_SAFE_BOOT_BIT);
    }
    // deinit the safe boot pin
    mperror_deinit_sfe_pin();
}

//*****************************************************************************
//! Load the proper image based on the information from the boot info
//! and launch it.
//*****************************************************************************
static void bootmgr_image_loader(sBootInfo_t *psBootInfo) {
    _i32 fhandle;
    _u8 *image;

    // search for the active image
    switch (psBootInfo->ActiveImg) {
    case IMG_ACT_UPDATE1:
        image = (unsigned char *)IMG_UPDATE1;
        break;
    case IMG_ACT_UPDATE2:
        image = (unsigned char *)IMG_UPDATE2;
        break;
    default:
        image = (unsigned char *)IMG_FACTORY;
        break;
    }

    // do we have a new image that needs to be verified?
    if ((psBootInfo->ActiveImg != IMG_ACT_FACTORY) && (psBootInfo->Status == IMG_STATUS_CHECK)) {
        if (!bootmgr_verify(image)) {
            // verification failed, delete the broken file
            sl_FsDel(image, 0);
            // switch to the previous image
            psBootInfo->ActiveImg = psBootInfo->PrevImg;
            psBootInfo->PrevImg = IMG_ACT_FACTORY;
        }
        // in any case, change the status to "READY"
        psBootInfo->Status = IMG_STATUS_READY;
        // write the new boot info
        if (!sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_WRITE, NULL, &fhandle)) {
            sl_FsWrite(fhandle, 0, (unsigned char *)psBootInfo, sizeof(sBootInfo_t));
            // close the file
            sl_FsClose(fhandle, 0, 0, 0);
        }
    }

    // this one might modify the boot info hence it MUST be called after
    // bootmgr_verify! (so that the changes are not saved to flash)
    wait_for_safe_boot(psBootInfo);

    // select the active image again, since it might have changed
    switch (psBootInfo->ActiveImg) {
    case IMG_ACT_UPDATE1:
        image = (unsigned char *)IMG_UPDATE1;
        break;
    case IMG_ACT_UPDATE2:
        image = (unsigned char *)IMG_UPDATE2;
        break;
    default:
        image = (unsigned char *)IMG_FACTORY;
        break;
    }
    bootmgr_load_and_execute(image);
}

//*****************************************************************************
//! Main function
//*****************************************************************************
int main (void) {
    sBootInfo_t sBootInfo = { .ActiveImg = IMG_ACT_FACTORY, .Status = IMG_STATUS_READY, .PrevImg = IMG_ACT_FACTORY };
    bool bootapp = false;
    _i32 fhandle;

    // board setup
    bootmgr_board_init();

    // start simplelink since we need it to access the sflash
    sl_Start(0, 0, 0);

    // if a boot info file is found, load it, else, create a new one with the default boot info
    if (!sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_READ, NULL, &fhandle)) {
        if (sizeof(sBootInfo_t) == sl_FsRead(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t))) {
            bootapp = true;
        }
        sl_FsClose(fhandle, 0, 0, 0);
    }
    // boot info file not present, it means that this is the first boot after being programmed
    if (!bootapp) {
        // create a new boot info file
        _u32 BootInfoCreateFlag  = _FS_FILE_OPEN_FLAG_COMMIT | _FS_FILE_PUBLIC_WRITE | _FS_FILE_PUBLIC_READ;
        if (!sl_FsOpen ((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_CREATE((2 * sizeof(sBootInfo_t)),
                        BootInfoCreateFlag), NULL, &fhandle)) {
            // write the default boot info.
            if (sizeof(sBootInfo_t) == sl_FsWrite(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t))) {
                bootapp = true;
            }
            sl_FsClose(fhandle, 0, 0, 0);
        }
        // signal the first boot to the application
        PRCMSetSpecialBit(PRCM_FIRST_BOOT_BIT);
    }

    if (bootapp) {
        // load and execute the image based on the boot info
        bootmgr_image_loader(&sBootInfo);
    }

    // stop simplelink
    sl_Stop(SL_STOP_TIMEOUT);

    // if we've reached this point, then it means that a fatal error has occurred and the
    // application could not be loaded, so, loop forever and signal the crash to the user
    while (true) {
        // keep the bld on
        MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, MICROPY_SYS_LED_PORT_PIN);
        __asm volatile(" dsb \n"
                       " isb \n"
                       " wfi \n");
    }
}

//*****************************************************************************
//! The following stub function is needed to link mp_vprintf
//*****************************************************************************
#include "py/qstr.h"

const byte *qstr_data(qstr q, size_t *len) {
    *len = 0;
    return NULL;
}
