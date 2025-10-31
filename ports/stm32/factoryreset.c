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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs_fat.h"
#include "systick.h"
#include "led.h"
#include "storage.h"
#include "factoryreset.h"

#if MICROPY_HW_ENABLE_STORAGE

#if MICROPY_VFS_FAT

static const char fresh_boot_py[] =
    "# boot.py -- run on boot to configure USB and filesystem\r\n"
    "# Put app code in main.py\r\n"
    "\r\n"
    "import machine\r\n"
    "import pyb\r\n"
    "#pyb.main('main.py') # main script to run after this one\r\n"
#if MICROPY_HW_ENABLE_USB
#if MICROPY_HW_TINYUSB_STACK
    "#usb = machine.USBDevice()\r\n"
    "#usb.builtin_driver = usb.BUILTIN_CDC | usb.BUILTIN_MSC  # serial and storage\r\n"
    "#usb.active(True)\r\n"
#if MICROPY_HW_NETWORK_USBNET
    "#usb.builtin_driver = usb.BUILTIN_CDC | usb.BUILTIN_NCM  # serial and network\r\n"
#endif
#else
    "#pyb.usb_mode('VCP+MSC') # act as a serial and a storage device\r\n"
    "#pyb.usb_mode('VCP+HID') # act as a serial device and a mouse\r\n"
#endif
#endif
#if MICROPY_PY_NETWORK
    "#import network\r\n"
    "#network.country('US') # ISO 3166-1 Alpha-2 code, eg US, GB, DE, AU or XX for worldwide\r\n"
    "#network.hostname('...') # DHCP/mDNS hostname\r\n"
#endif
;

static const char fresh_main_py[] =
    "# main.py -- put your code here!\r\n"
;

#if MICROPY_HW_ENABLE_USB
static const char fresh_pybcdc_inf[] =
#include "genhdr/pybcdc_inf.h"
;

static const char fresh_readme_txt[] =
    "This is a MicroPython board\r\n"
    "\r\n"
    "You can get started right away by writing your Python code in 'main.py'.\r\n"
    "\r\n"
    "For a serial prompt:\r\n"
    " - Windows: you need to go to 'Device manager', right click on the unknown device,\r\n"
    "   then update the driver software, using the 'pybcdc.inf' file found on this drive.\r\n"
    "   Then use a terminal program like Hyperterminal or putty.\r\n"
    " - Mac OS X: use the command: screen /dev/tty.usbmodem*\r\n"
    " - Linux: use the command: screen /dev/ttyACM0\r\n"
    "\r\n"
    "For online docs please visit http://docs.micropython.org/\r\n"
;
#endif

typedef struct _factory_file_t {
    const char *name;
    size_t len;
    const char *data;
} factory_file_t;

static const factory_file_t factory_files[] = {
    {"boot.py", sizeof(fresh_boot_py) - 1, fresh_boot_py},
    {"main.py", sizeof(fresh_main_py) - 1, fresh_main_py},
    #if MICROPY_HW_ENABLE_USB
    {"pybcdc.inf", sizeof(fresh_pybcdc_inf) - 1, fresh_pybcdc_inf},
    {"README.txt", sizeof(fresh_readme_txt) - 1, fresh_readme_txt},
    #endif
};

MP_WEAK void factory_reset_make_files(FATFS *fatfs) {
    for (int i = 0; i < MP_ARRAY_SIZE(factory_files); ++i) {
        const factory_file_t *f = &factory_files[i];
        FIL fp;
        FRESULT res = f_open(fatfs, &fp, f->name, FA_WRITE | FA_CREATE_ALWAYS);
        if (res == FR_OK) {
            UINT n;
            f_write(&fp, f->data, f->len, &n);
            f_close(&fp);
        }
    }
}

MP_WEAK int factory_reset_create_filesystem(void) {
    // LED on to indicate creation of local filesystem
    led_state(PYB_LED_GREEN, 1);
    uint32_t start_tick = HAL_GetTick();

    fs_user_mount_t vfs;
    vfs.blockdev.flags = 0;
    pyb_flash_init_vfs(&vfs);
    uint8_t working_buf[FF_MAX_SS];
    FRESULT res = f_mkfs(&vfs.fatfs, FM_FAT, 0, working_buf, sizeof(working_buf));
    if (res != FR_OK) {
        mp_printf(&mp_plat_print, "MPY: can't create flash filesystem\n");
        return -MP_ENODEV;
    }

    // Set label
    f_setlabel(&vfs.fatfs, MICROPY_HW_FLASH_FS_LABEL);

    // Populate the filesystem with factory files
    factory_reset_make_files(&vfs.fatfs);

    // Keep LED on for at least 200ms
    systick_wait_at_least(start_tick, 200);
    led_state(PYB_LED_GREEN, 0);

    return 0; // success
}

#else

// If FAT is not enabled then it's up to the board to create a fresh filesystem.
MP_WEAK int factory_reset_create_filesystem(void) {
    return 0; // success
}

#endif

#endif // MICROPY_HW_ENABLE_STORAGE
