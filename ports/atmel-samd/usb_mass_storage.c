/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "usb_mass_storage.h"
#include "supervisor/shared/autoreload.h"

#include "hal/utils/include/err_codes.h"
#include "hal/utils/include/utils.h"
#include "usb/class/msc/device/mscdf.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "lib/oofatfs/ffconf.h"
#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/misc.h"

// The root FS is always at the end of the list.
static fs_user_mount_t* get_vfs(int lun) {
    // TODO(tannewt): Return the mount which matches the lun where 0 is the end
    // and is counted in reverse.
    if (lun > 0) {
        return NULL;
    }
    mp_vfs_mount_t* current_mount = MP_STATE_VM(vfs_mount_table);
    if (current_mount == NULL) {
        return NULL;
    }
    while (current_mount->next != NULL) {
        current_mount = current_mount->next;
    }
    return current_mount->obj;
}

/* Inquiry Information */
// This is designed to handle the common case where we have an internal file
// system and an optional SD card.
COMPILER_ALIGNED(4) static uint8_t inquiry_info[2][36];

/* Capacities of Disk */
COMPILER_ALIGNED(4) static uint8_t format_capa[2][8];

/**
 * \brief Eject Disk
 * \param[in] lun logic unit number
 * \return Operation status.
 */
int32_t usb_msc_disk_eject(uint8_t lun) {
    if (lun > 1) {
        return ERR_NOT_FOUND;
    }
    fs_user_mount_t* current_mount = get_vfs(lun);
    // Return ERR_NOT_READY if not ready, otherwise ERR_NONE.
    if (current_mount == NULL) {
        return ERR_NOT_FOUND;
    }
    // TODO(tannewt): Should we flush here?
    return ERR_NONE;
}

/**
 * \brief Inquiry whether Disk is writable. ERR_DENIED if it is not writable.
 *     ERR_NONE if it is. ERR_NOT_FOUND if its missing.
 * \param[in] lun logic unit number
 * \return Operation status.
 */
int32_t usb_msc_disk_is_writable(uint8_t lun) {
    if (lun > 1) {
        return ERR_NOT_FOUND;
    }

    fs_user_mount_t* vfs = get_vfs(lun);
    if (vfs == NULL) {
        return ERR_NOT_FOUND;
    }
    if (vfs->writeblocks[0] == MP_OBJ_NULL ||
        (vfs->flags & FSUSER_USB_WRITABLE) == 0) {
        return ERR_DENIED;
    }
    return ERR_NONE;
}

/**
 * \brief Inquiry whether Disk is ready
 * \param[in] lun logic unit number
 * \return Operation status.
 */
int32_t usb_msc_disk_is_ready(uint8_t lun) {
    if (lun > 1) {
        return ERR_NOT_FOUND;
    }

    fs_user_mount_t* current_mount = get_vfs(lun);
    if (current_mount == NULL) {
        return ERR_NOT_FOUND;
    }
    return ERR_NONE;
}

/**
 * \brief Callback invoked when inquiry data command received
 * \param[in] lun logic unit number
 * \return Operation status.
 */
uint8_t *usb_msc_inquiry_info(uint8_t lun) {
    if (lun > 1) {
        return NULL;
    } else {
        for (uint8_t i = 0; i < 36; i++) {
            inquiry_info[lun][i] = 0;
        }
        inquiry_info[lun][0] = SCSI_INQ_PQ_CONNECTED | SCSI_INQ_DT_DIR_ACCESS;
                               // connected, direct access
        inquiry_info[lun][1] = SCSI_INQ_RMB;     // removable medium
        inquiry_info[lun][2] = SCSI_INQ_VER_SPC; // SBC version of SCSI primary commands
        inquiry_info[lun][3] = SCSI_INQ_RSP_SPC2;// SPC-2 response format
        inquiry_info[lun][4] = 31;               // 31 bytes following
        return &inquiry_info[lun][0];
    }
}

/**
 * \brief Callback invoked when read format capacities command received
 * \param[in] lun logic unit number
 */
uint8_t *usb_msc_get_capacity(uint8_t lun) {
    if (lun > 1) {
        return NULL;
    } else {
        fs_user_mount_t * vfs = get_vfs(lun);
        uint32_t last_valid_sector = 0;
        uint32_t sector_size = 0;
        if (vfs == NULL ||
            disk_ioctl(vfs, GET_SECTOR_COUNT, &last_valid_sector) != RES_OK ||
            disk_ioctl(vfs, GET_SECTOR_SIZE, &sector_size) != RES_OK) {
            return NULL;
        }
        // Subtract one from the sector count to get the last valid sector.
        last_valid_sector--;

        format_capa[lun][0] = (uint8_t)(last_valid_sector >> 24);
        format_capa[lun][1] = (uint8_t)(last_valid_sector >> 16);
        format_capa[lun][2] = (uint8_t)(last_valid_sector >> 8);
        format_capa[lun][3] = (uint8_t)(last_valid_sector >> 0);
        format_capa[lun][4] = (uint8_t)(sector_size >> 24);
        format_capa[lun][5] = (uint8_t)(sector_size >> 16);
        format_capa[lun][6] = (uint8_t)(sector_size >> 8);
        format_capa[lun][7] = (uint8_t)(sector_size >> 0);

        // 8 byte response. First 4 bytes are last block address. Second 4
        // bytes are sector size.
        return &format_capa[lun][0];
    }
}

// USB transfer state.
volatile bool usb_busy;
volatile bool active_read;
volatile bool active_write;
volatile uint8_t active_lun;
volatile uint32_t active_addr;
volatile uint32_t active_nblocks;
volatile bool sector_loaded;
COMPILER_ALIGNED(4) uint8_t sector_buffer[512];

/**
 * \brief Callback invoked when a new read blocks command received
 * \param[in] lun logic unit number
 * \param[in] addr start address of disk to be read
 * \param[in] nblocks block amount to be read
 * \return Operation status.
 */
int32_t usb_msc_new_read(uint8_t lun, uint32_t addr, uint32_t nblocks) {
    if (lun > 1) {
        return ERR_NOT_FOUND;
    }

    // Store transfer info so we can service it in the "background".
    active_lun = lun;
    active_addr = addr;
    active_nblocks = nblocks;
    active_read = true;

    return ERR_NONE;
}

/**
 * \brief Callback invoked when a new write blocks command received
 * \param[in] lun logic unit number
 * \param[in] addr start address of disk to be written
 * \param[in] nblocks block amount to be written
 * \return Operation status.
 */
int32_t usb_msc_new_write(uint8_t lun, uint32_t addr, uint32_t nblocks) {
    if (lun > 1) {
        return ERR_NOT_FOUND;
    }

    fs_user_mount_t * vfs = get_vfs(lun);
    // This is used to determine the writeability of the disk from USB.
    if (vfs == NULL) {
        return ERR_NOT_FOUND;
    }
    if (vfs->writeblocks[0] == MP_OBJ_NULL ||
        (vfs->flags & FSUSER_USB_WRITABLE) == 0) {
        return ERR_DENIED;
    }

    // Store transfer info so we can service it in the "background".
    active_lun = lun;
    active_addr = addr;
    active_nblocks = nblocks;
    active_write = true;
    sector_loaded = false;

    // Return ERR_DENIED when the file system is read-only to the USB host.

    return ERR_NONE;
}

/**
 * \brief Callback invoked when a blocks transfer is done
 * \param[in] lun logic unit number
 * \return Operation status.
 */
int32_t usb_msc_xfer_done(uint8_t lun) {
    if (lun > 1) {
        return ERR_DENIED;
    }

    CRITICAL_SECTION_ENTER();
    if (active_read) {
        active_addr += 1;
        active_nblocks--;
        if (active_nblocks == 0) {
            active_read = false;
        }
    }

    if (active_write) {
        sector_loaded = true;
    }
    usb_busy = false;
    CRITICAL_SECTION_LEAVE();

    return ERR_NONE;
}

// The start_read callback begins a read transaction which we accept
// but delay our response until the "main thread" calls
// usb_msc_background. Once it does, we read immediately from the
// drive into our cache and trigger the USB DMA to output the
// sector. Once the sector is transmitted, xfer_done will be called.
void usb_msc_background(void) {
    // Check USB busy first because we never want to queue another transfer if it is. Checking
    // active_read or active_write first leaves the possibility that they are true, an xfer done
    // interrupt occurs (setting them false), turning off usb_busy and causing us to queue a
    // spurious transfer.
    if (usb_busy) {
        return;
    }
    if (active_read) {
        fs_user_mount_t * vfs = get_vfs(active_lun);
        disk_read(vfs, sector_buffer, active_addr, 1);
        CRITICAL_SECTION_ENTER();
        int32_t result = mscdf_xfer_blocks(true, sector_buffer, 1);
        usb_busy = result == ERR_NONE;
        CRITICAL_SECTION_LEAVE();
    }
    if (active_write) {
        if (sector_loaded) {
            fs_user_mount_t * vfs = get_vfs(active_lun);
            disk_write(vfs, sector_buffer, active_addr, 1);
            // Since by getting here we assume the mount is read-only to
            // MicroPython let's update the cached FatFs sector if it's the one
            // we just wrote.
            #if _MAX_SS != _MIN_SS
            if (vfs->ssize == FILESYSTEM_BLOCK_SIZE) {
            #else
            // The compiler can optimize this away.
            if (_MAX_SS == FILESYSTEM_BLOCK_SIZE) {
            #endif
                if (active_addr == vfs->fatfs.winsect && active_addr > 0) {
                    memcpy(vfs->fatfs.win,
                           sector_buffer,
                           FILESYSTEM_BLOCK_SIZE);
                }
            }
            sector_loaded = false;
            active_addr += 1;
            active_nblocks--;
        }
        // Load more blocks from USB if they are needed.
        if (active_nblocks > 0) {
            // Turn off interrupts because with them on,
            // usb_msc_xfer_done could be called before we update
            // usb_busy. If that happened, we'd overwrite the fact that
            // the transfer actually already finished.
            CRITICAL_SECTION_ENTER();
            int32_t result = mscdf_xfer_blocks(false, sector_buffer, 1);
            usb_busy = result == ERR_NONE;
            CRITICAL_SECTION_LEAVE();
        } else {
            mscdf_xfer_blocks(false, NULL, 0);
            active_write = false;
            // This write is complete, start the autoreload clock.
            autoreload_start();
        }
    }
}
