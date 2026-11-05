// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "tusb.h"

// For updating fatfs's cache
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/diskio.h"
#include "lib/oofatfs/ff.h"
#include "py/gc.h"
#include "py/mpstate.h"
#include "py/runtime.h"

#include "shared-module/storage/__init__.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/reload.h"
#include "supervisor/shared/settings.h"

#define MSC_FLASH_BLOCK_SIZE    512

#if CIRCUITPY_SAVES_PARTITION_SIZE > 0
#define SAVES_COUNT 1
#define SAVES_LUN (1)
#else
#define SAVES_COUNT 0
#endif

#if CIRCUITPY_SDCARD_USB
#include "shared-module/sdcardio/__init__.h"

#define SDCARD_COUNT 1
#define SDCARD_LUN (1 + SAVES_COUNT)
#else
#define SDCARD_COUNT 0
#endif

#if CIRCUITPY_EMMC_USB
#include "shared-module/emmcio/__init__.h"

#define EMMC_COUNT 1
#define EMMC_LUN (1 + SAVES_COUNT + SDCARD_COUNT)
#else
#define EMMC_COUNT 0
#endif

#define LUN_COUNT (1 + SAVES_COUNT + SDCARD_COUNT + EMMC_COUNT)

// The ellipsis range in the designated initializer of `ejected` is not standard C,
// but it works in both gcc and clang.
static bool ejected[LUN_COUNT] = { [0 ... (LUN_COUNT - 1)] = true};
static bool eject_once[LUN_COUNT] = { [0 ... (LUN_COUNT - 1)] = false};
static bool locked[LUN_COUNT] = { [0 ... (LUN_COUNT - 1)] = false};

// Set to true if a write was in a file data or metadata area,
// as opposed to in the filesystem metadata area (e.g., dirty bit).
// Used to determine if an auto-reload is warranted.
static bool content_write[LUN_COUNT] = { [0 ... (LUN_COUNT - 1)] = false};

#include "tusb.h"

static const uint8_t usb_msc_descriptor_template[] = {
    // MSC Interface Descriptor
    0x09,        //  0 bLength
    0x04,        //  1 bDescriptorType (Interface)
    0xFF,        //  2 bInterfaceNumber [SET AT RUNTIME]
#define MSC_INTERFACE_INDEX (2)
    0x00,        //  3 bAlternateSetting
    0x02,        //  4 bNumEndpoints 2
    0x08,        //  5 bInterfaceClass: MSC
    0x06,        //  6 bInterfaceSubClass: TRANSPARENT
    0x50,        //  7 bInterfaceProtocol: BULK
    0xFF,        //  8 iInterface (String Index) [SET AT RUNTIME]
#define MSC_INTERFACE_STRING_INDEX (8)

    // MSC Endpoint IN Descriptor
    0x07,        //  9 bLength
    0x05,        // 10 bDescriptorType (Endpoint)
    0xFF,        // 11 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | number]
#define MSC_IN_ENDPOINT_INDEX (11)
    0x02,        // 12 bmAttributes (Bulk)
    #if CIRCUITPY_USB_DEVICE_HIGH_SPEED == 1
    0x00, 0x02,  // 13,14 wMaxPacketSize 512
    #else
    0x40, 0x00,  // 13,14 wMaxPacketSize 64
    #endif
    0x00,        // 15 bInterval 0 (unit depends on device speed)

    // MSC Endpoint OUT Descriptor
    0x07,        // 16 bLength
    0x05,        // 17 bDescriptorType (Endpoint)
    0xFF,        // 18 bEndpointAddress (OUT/H2D) [SET AT RUNTIME]
#define MSC_OUT_ENDPOINT_INDEX (18)
    0x02,        // 19 bmAttributes (Bulk)
    #if CIRCUITPY_USB_DEVICE_HIGH_SPEED == 1
    0x00, 0x02,  // 20,21 wMaxPacketSize 512
    #else
    0x40, 0x00,  // 20,21 wMaxPacketSize 64
    #endif
    0x00,        // 22 bInterval 0 (unit depends on device speed)
};

size_t usb_msc_descriptor_length(void) {
    return sizeof(usb_msc_descriptor_template);
}

static const char storage_interface_name[] = "Mass Storage";

size_t usb_msc_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string) {
    memcpy(descriptor_buf, usb_msc_descriptor_template, sizeof(usb_msc_descriptor_template));
    descriptor_buf[MSC_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    descriptor_buf[MSC_IN_ENDPOINT_INDEX] =
        0x80 | (USB_MSC_EP_NUM_IN ? USB_MSC_EP_NUM_IN : descriptor_counts->current_endpoint);
    descriptor_counts->num_in_endpoints++;
    // Some TinyUSB devices have issues with bi-directional endpoints
    #ifdef TUD_ENDPOINT_ONE_DIRECTION_ONLY
    descriptor_counts->current_endpoint++;
    #endif

    descriptor_buf[MSC_OUT_ENDPOINT_INDEX] =
        USB_MSC_EP_NUM_OUT ? USB_MSC_EP_NUM_OUT : descriptor_counts->current_endpoint;
    descriptor_counts->num_out_endpoints++;
    descriptor_counts->current_endpoint++;

    usb_add_interface_name(*current_interface_string, NULL, storage_interface_name);
    descriptor_buf[MSC_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    return sizeof(usb_msc_descriptor_template);
}

// We hardcode LUN -> mount mapping so that it doesn't changes with saves and
// SD card appearing and disappearing.
static fs_user_mount_t *get_vfs(int lun) {
    supervisor_vfs_t *root_vfs = filesystem_circuitpy();
    fs_user_mount_t *root = root_vfs == NULL ? NULL : &root_vfs->fat;
    if (lun == 0) {
        return root;
    }
    // Other filesystems must be native because we don't guard against exceptions.
    // They must also be off the VM heap so they don't disappear on autoreload.
    #ifdef SAVES_LUN
    if (lun == SAVES_LUN) {
        const char *path_under_mount;
        supervisor_vfs_t *saves_mount = filesystem_for_path("/saves", &path_under_mount);
        fs_user_mount_t *saves = saves_mount == NULL ? NULL : &saves_mount->fat;
        if (saves != root &&
            (saves->blockdev.flags & MP_BLOCKDEV_FLAG_NATIVE) != 0 && !gc_ptr_on_heap(saves)) {
            return saves;
        }
    }
    #endif
    #ifdef SDCARD_LUN
    if (lun == SDCARD_LUN) {
        const char *path_under_mount;
        supervisor_vfs_t *sdcard_mount = filesystem_for_path("/sd", &path_under_mount);
        fs_user_mount_t *sdcard = sdcard_mount == NULL ? NULL : &sdcard_mount->fat;
        // If sdcard ("/sd") is on the root filesystem, nothing has been mounted there, so don't
        // return it as a separate filesystem.
        // If the SD card was automounted at startup, then it persists across VMs and its fs_user_mount_t is
        // not on the heap.
        // If the SD card filesystem was mounted by the user using heap objects,
        // it should not be used when the VM has stopped running.
        if ((sdcard != root) &&
            ((sdcard->blockdev.flags & MP_BLOCKDEV_FLAG_NATIVE) != 0) &&
            (vm_is_running() || !gc_ptr_on_heap(sdcard))) {
            return sdcard;
        } else {
            // Clear any ejected state so that a re-insert causes it to reappear.
            ejected[SDCARD_LUN] = false;
            locked[SDCARD_LUN] = false;
        }
    }
    #endif
    #ifdef EMMC_LUN
    if (lun == EMMC_LUN) {
        const char *path_under_mount;
        fs_user_mount_t *emmc;

        supervisor_vfs_t *emmc_mount = filesystem_for_path(CIRCUITPY_EMMC_MOUNT_PATH, &path_under_mount);
        emmc = emmc_mount == NULL ? NULL : &emmc_mount->fat;
        // Unlike the SD card there is no heap-mount case to allow: the eMMC's
        // drive exists only when the supervisor mounted it, and
        // that mount is static. A user mount made by code.py stays a Python
        // filesystem and never becomes a LUN.
        if (emmc != root &&
            ((emmc->blockdev.flags & MP_BLOCKDEV_FLAG_NATIVE) != 0) &&
            !gc_ptr_on_heap(emmc)) {
            return emmc;
        } else {
            // Clear any ejected state so that a remount causes it to reappear.
            ejected[EMMC_LUN] = false;
            locked[EMMC_LUN] = false;
        }
    }
    #endif
    return NULL;
}

static void _usb_msc_uneject(void) {
    for (uint8_t i = 0; i < LUN_COUNT; i++) {
        ejected[i] = false;
        locked[i] = false;
    }
}

void usb_msc_mount(void) {
    _usb_msc_uneject();
}

void usb_msc_umount(void) {
    for (uint8_t i = 0; i < LUN_COUNT; i++) {
        fs_user_mount_t *vfs = get_vfs(i);
        if (vfs == NULL) {
            continue;
        }
        blockdev_unlock((supervisor_vfs_t *)vfs);
        locked[i] = false;
    }
}

// Release the USB MSC lock on CIRCUITPY (LUN 0), if USB MSC holds it.
// Locks held by others (Python, workflows) are not touched.
void usb_msc_release_circuitpy_lock(void) {
    if (!locked[0]) {
        return;
    }
    fs_user_mount_t *vfs = get_vfs(0);
    if (vfs != NULL) {
        blockdev_unlock((supervisor_vfs_t *)vfs);
    }
    locked[0] = false;
}

// Whether USB MSC holds the blockdev lock on this mount.
bool usb_msc_holds_lock(fs_user_mount_t *vfs) {
    for (uint8_t i = 0; i < LUN_COUNT; i++) {
        if (locked[i] && get_vfs(i) == vfs) {
            return true;
        }
    }
    return false;
}

void usb_msc_remount(fs_user_mount_t *fs_mount) {
    for (uint8_t i = 0; i < LUN_COUNT; i++) {
        fs_user_mount_t *vfs = get_vfs(i);
        if (vfs == NULL || vfs != fs_mount) {
            continue;
        }
        ejected[i] = false;
        eject_once[i] = true;
    }
}

uint8_t tud_msc_get_maxlun_cb(void) {
    return LUN_COUNT;
}

// PREVENT ALLOW MEDIUM REMOVAL: TinyUSB routes this to its own dedicated callback,
// NOT tud_msc_scsi_cb. Reply unsupported on all LUNs so the host keeps TUR polling
// on every LUN — eject/storage.remount() works uniformly across CIRCUITPY, SAVES, SD.
bool tud_msc_prevent_allow_medium_removal_cb(uint8_t lun, uint8_t prohibit_removal, uint8_t control) {
    (void)prohibit_removal;
    (void)control;
    tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
    return false;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, TEST_UNIT_READY, START_STOP_UNIT, MODE_SENSE6, REQUEST_SENSE
// - PREVENT_ALLOW_MEDIUM_REMOVAL, READ10, WRITE10 have their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, const uint8_t scsi_cmd[16], void *buffer, uint16_t bufsize) {
    // Note that no command uses a response right now.
    const void *response = NULL;
    int32_t resplen = 0;

    switch (scsi_cmd[0]) {
        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            // negative means error -> tinyusb could stall and/or response with failed status
            resplen = -1;
            break;
    }

    // return len must not larger than bufsize
    if (resplen > bufsize) {
        resplen = bufsize;
    }

    // copy response to stack's buffer if any
    if (response && (resplen > 0)) {
        memcpy(buffer, response, resplen);
    }

    return resplen;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
    fs_user_mount_t *vfs = get_vfs(lun);
    if (vfs != NULL) {
        disk_ioctl(vfs, GET_SECTOR_COUNT, block_count);
        disk_ioctl(vfs, GET_SECTOR_SIZE, block_size);
    }
}

bool tud_msc_is_writable_cb(uint8_t lun) {
    if (lun >= LUN_COUNT) {
        return false;
    }

    fs_user_mount_t *vfs = get_vfs(lun);
    if (vfs == NULL) {
        return false;
    }
    if (!filesystem_is_writable_by_usb(vfs)) {
        return false;
    }
    // Lock the blockdev once we say we're writable.
    if (!locked[lun] && !blockdev_lock((supervisor_vfs_t *)vfs)) {
        return false;
    }
    locked[lun] = true;
    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
    (void)offset;

    const uint32_t block_count = bufsize / MSC_FLASH_BLOCK_SIZE;

    fs_user_mount_t *vfs = get_vfs(lun);
    if (vfs == NULL) {
        return -1;
    }
    uint32_t disk_block_count;
    disk_ioctl(vfs, GET_SECTOR_COUNT, &disk_block_count);

    if (lba + block_count > disk_block_count) {
        return -1;
    }

    if (disk_read(vfs, buffer, lba, block_count) != RES_OK) {
        // TinyUSB fails the command and reports MEDIUM NOT PRESENT sense.
        return TUD_MSC_RET_ERROR;
    }

    return block_count * MSC_FLASH_BLOCK_SIZE;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
    (void)lun;
    (void)offset;
    autoreload_suspend(AUTORELOAD_SUSPEND_USB);

    const uint32_t block_count = bufsize / MSC_FLASH_BLOCK_SIZE;

    fs_user_mount_t *vfs = get_vfs(lun);
    if (vfs == NULL) {
        return -1;
    }

    if (disk_write(vfs, buffer, lba, block_count) != RES_OK) {
        // TinyUSB fails the command and reports MEDIUM NOT PRESENT sense.
        return TUD_MSC_RET_ERROR;
    }
    // Since by getting here we assume the mount is read-only to
    // CircuitPython let's update the cached FatFs sector if it's the one
    // we just wrote.
    if
    #if FF_MAX_SS != FF_MIN_SS
    (vfs->fatfs.ssize == MSC_FLASH_BLOCK_SIZE)
    #else
    // The compiler can optimize this away.
    (FF_MAX_SS == FILESYSTEM_BLOCK_SIZE)
    #endif
    {
        if (lba == vfs->fatfs.winsect && lba > 0) {
            memcpy(vfs->fatfs.win,
                buffer + MSC_FLASH_BLOCK_SIZE * (vfs->fatfs.winsect - lba),
                MSC_FLASH_BLOCK_SIZE);
        }
    }

    // A write to an lba below fatbase is in the filesystem metadata (BPB) area or the "Reserved Region",
    // and is probably setting or clearing the dirty bit. This should not trigger auto-reload.
    // All other writes will trigger auto-reload.
    if (lba >= vfs->fatfs.fatbase) {
        content_write[lun] = true;
    }

    return block_count * MSC_FLASH_BLOCK_SIZE;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void tud_msc_write10_complete_cb(uint8_t lun) {
    autoreload_resume(AUTORELOAD_SUSPEND_USB);

    // This write is complete; initiate an autoreload if this was a file data or metadata write,
    // not just a dirty-bit write.
    if (content_write[lun] &&
        // Fast path: lun == 0 is CIRCUITPY, which can always trigger auto-reload if enabled.
        // Don't autoreload if this lun was mounted by the user: that will cause a VM stop and an unmount.
        (lun == 0 || !gc_ptr_on_heap(get_vfs(lun)))) {
        autoreload_trigger();
        content_write[lun] = false;
    }
}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    (void)lun;

    memcpy(vendor_id, CFG_TUD_MSC_VENDOR, strlen(CFG_TUD_MSC_VENDOR));
    memcpy(product_id, CFG_TUD_MSC_PRODUCT, strlen(CFG_TUD_MSC_PRODUCT));
    memcpy(product_rev, CFG_TUD_MSC_PRODUCT_REV, strlen(CFG_TUD_MSC_PRODUCT_REV));
}

#ifdef SDCARD_LUN
#if CIRCUITPY_SETTINGS_TOML
typedef enum {
    SDCARD_USB_SETTING_NOT_YET_READ = 0,
    SDCARD_USB_SETTING_TRUE,
    SDCARD_USB_SETTING_FALSE,
} sdcard_usb_setting_state_t;

static sdcard_usb_setting_state_t _sdcard_usb_setting_state = SDCARD_USB_SETTING_NOT_YET_READ;

// Read only once to save file access time.
static bool sdcard_usb_enabled(void) {
    if (_sdcard_usb_setting_state == SDCARD_USB_SETTING_NOT_YET_READ) {
        // Can be changed per board. Most boards would leave this as true (default in circuitpy_mpconfig.h).
        bool setting = CIRCUITPY_SDCARD_USB_DEFAULT;
        (void)settings_get_bool("CIRCUITPY_SDCARD_USB", &setting);
        _sdcard_usb_setting_state = setting ? SDCARD_USB_SETTING_TRUE : SDCARD_USB_SETTING_FALSE;
    }
    return _sdcard_usb_setting_state == SDCARD_USB_SETTING_TRUE;
}
#else
static bool sdcard_usb_enabled(void) {
    return CIRCUITPY_SDCARD_USB;
}
#endif
#endif

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    if (lun >= LUN_COUNT) {
        return false;
    }

    fs_user_mount_t *current_mount = get_vfs(lun);
    if (current_mount == NULL) {
        return false;
    }

    if (ejected[lun] || eject_once[lun] || (lun == 0 && !storage_usb_enabled())
        #ifdef SDCARD_LUN
        || (lun == SDCARD_LUN && !sdcard_usb_enabled())
        #endif
        ) {
        eject_once[lun] = false;
        // Set 0x3a for media not present.
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
        return false;
    }

    return true;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
    if (lun >= LUN_COUNT) {
        return false;
    }
    fs_user_mount_t *current_mount = get_vfs(lun);
    if (current_mount == NULL) {
        return false;
    }
    if (load_eject) {
        if (!start) {
            // Eject but first flush.
            if (disk_ioctl(current_mount, CTRL_SYNC, NULL) != RES_OK) {
                return false;
            } else {
                blockdev_unlock((supervisor_vfs_t *)current_mount);
                ejected[lun] = true;
                locked[lun] = false;
            }
        } else {
            // We can only load if it hasn't been ejected.
            return !ejected[lun];
        }
    } else {
        if (!start) {
            // Stop the unit but don't eject.
            if (disk_ioctl(current_mount, CTRL_SYNC, NULL) != RES_OK) {
                return false;
            }
        }
        // Always start the unit, even if ejected. Whether media is present is a separate check.
    }

    return true;
}
