/*****************************************************************************
*
* File Name : main.c
*
* Description: main
*
* Copyright (c) 2019 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*
*****************************************************************************/
#include "wm_include.h"
#include "wm_mem.h"
#include "wm_watchdog.h"

#include "py/stackctrl.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"

#include "mpthreadport.h"

OS_STK mpy_task_stk[MPY_TASK_SIZE];

#if MICROPY_USE_INTERVAL_FLS_FS
fs_user_mount_t *spi_fls_vfs_fat = NULL;

static const char fresh_boot_py[] = "# boot.py -- run on boot-up\r\n"
                                    "# can run arbitrary Python, but best to keep it minimal\r\n"
                                    "print(\"\")\r\n"
                                    "print(\"    WinnerMicro W600\")\r\n"
                                    "print(\"\")\r\n";
static const char fresh_main_py[] = "# main.py -- put your code here!\r\n";
static const char fresh_easyw600_py[] = "# easyw600.py -- w600 example script\r\n"
                                        "import binascii\r\n"
                                        "import network\r\n"
                                        "import utime\r\n"
                                        "import w600\r\n"
                                        "\r\n"
                                        "def scan():\r\n"
                                        "    sta_if = network.WLAN(network.STA_IF)\r\n"
                                        "    sta_if.active(True)\r\n"
                                        "    for i in sta_if.scan():\r\n"
                                        "        print(\"%32s\"%(bytes(i[0]).decode('ascii')+'\\t'),bytes(binascii.hexlify(i[1])).decode('ascii')+\"\\t\",str(i[2])+\"\\t\",str(i[3])+\"\\t\",str(i[4])+\"\\t\",i[5])\r\n"
                                        "\r\n"
                                        "def oneshot():\r\n"
                                        "    sta_if = network.WLAN(network.STA_IF)\r\n"
                                        "    sta_if.active(True)\r\n"
                                        "    sta_if.oneshot(1)\r\n"
                                        "    print(\"wait conneting...\")\r\n"
                                        "    while (1):\r\n"
                                        "        if (sta_if.isconnected()):\r\n"
                                        "            break;\r\n"
                                        "        else:\r\n"
                                        "            utime.sleep(1)\r\n"
                                        "    print(\"connected, ip is \" + sta_if.ifconfig()[0])\r\n"
                                        "\r\n"
                                        "def connect(ssid=None, password=None):\r\n"
                                        "    sta_if = network.WLAN(network.STA_IF)\r\n"
                                        "    sta_if.active(True)\r\n"
                                        "    sta_if.connect(ssid, password)\r\n"
                                        "    print(\"conneting...\")\r\n"
                                        "    while (1):\r\n"
                                        "        if (sta_if.isconnected()):\r\n"
                                        "            break;\r\n"
                                        "        else:\r\n"
                                        "            utime.sleep(1)\r\n"
                                        "    print(\"connected, ip is \" + sta_if.ifconfig()[0])\r\n"
                                        "\r\n"
                                        "def disconnect():\r\n"
                                        "    sta_if = network.WLAN(network.STA_IF)\r\n"
                                        "    sta_if.disconnect()\r\n"
                                        "\r\n"
                                        "def createap(ssid=\"w600_softap\", passwd=None):\r\n"
                                        "    ap_if = network.WLAN(network.AP_IF)\r\n"
                                        "    ap_if.active(True)\r\n"
                                        "    if passwd is None:\r\n"
                                        "        ap_if.config(essid=ssid,authmode=network.AUTH_OPEN,channel=11)\r\n"
                                        "    else:\r\n"
                                        "        ap_if.config(essid=ssid,password=passwd,authmode=network.AUTH_WPA2_PSK_AES,channel=11)\r\n"
                                        "    print(\"softap working, ip is 192.168.43.1\")\r\n"
                                        "\r\n"
                                        "def closeap():\r\n"
                                        "    ap_if = network.WLAN(network.AP_IF)\r\n"
                                        "    ap_if.active(False)\r\n"
                                        "\r\n"
                                        "def ftpserver():\r\n"
                                        "    w600.run_ftpserver(port=21,username=\"root\",password=\"root\")\r\n"
                                        "    print(\"ftp server port is 21, username is root, password is root\")\r\n";


static void init_spiflash_fs (void) {
    FILINFO fno;
    FIL fp;

    // Initialise the local flash filesystem.
    // init the vfs object
    spi_fls_vfs_fat = tls_mem_alloc(sizeof(*spi_fls_vfs_fat));
    if (!spi_fls_vfs_fat) {
        printf("failed to init filesystem\r\n");
        return;
    }

    fs_user_mount_t *vfs_fat = spi_fls_vfs_fat;
    vfs_fat->flags = 0;
    w600_flash_init_vfs(vfs_fat);

    // Create it if needed, and mount in on /flash.
    FRESULT res = f_mount(&vfs_fat->fatfs);
    if (res == FR_NO_FILESYSTEM) {
        // no filesystem, so create a fresh one
        uint8_t working_buf[FF_MAX_SS];
        res = f_mkfs(&vfs_fat->fatfs, FM_FAT, 0, working_buf, sizeof(working_buf));
        if (res == FR_OK) {
            // success creating fresh LFS
        } else {
            printf("failed to create filesystem, err = %d\r\n", res);
            return;
        }

        // set label
        //f_setlabel(&vfs_fat->fatfs, "w600flash");
    } else if (res == FR_OK) {
        // mount sucessful

    } else {
fail:
        printf("failed to mount flash\r\n");
        return;
    }

    // mount the flash device (there should be no other devices mounted at this point)
    // we allocate this structure on the heap because vfs->next is a root pointer
    mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
    if (vfs == NULL) {
        goto fail;
    }
    vfs->str = "/flash";
    vfs->len = 6;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
    vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = vfs;

    // create /flash/sys, /flash/lib and /flash/cert if they don't exist
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/sys")) {
        f_mkdir(&vfs_fat->fatfs, "/sys");
    }
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/lib")) {
        f_mkdir(&vfs_fat->fatfs, "/lib");
    }
    if (FR_OK != f_chdir(&vfs_fat->fatfs, "/cert")) {
        f_mkdir(&vfs_fat->fatfs, "/cert");
    }

    f_chdir(&vfs_fat->fatfs, "/");

    // Make sure we have a /flash/boot.py.  Create it if needed.
    res = f_stat(&vfs_fat->fatfs, "/boot.py", &fno);
    if (res != FR_OK) { // doesn't exist, create fresh file
        FIL fp;
        f_open(&vfs_fat->fatfs, &fp, "/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }

    res = f_stat(&vfs_fat->fatfs, "/main.py", &fno);
    if (res != FR_OK) {
        // create empty main.py
        f_open(&vfs_fat->fatfs, &fp, "/main.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }

    res = f_stat(&vfs_fat->fatfs, "/easyw600.py", &fno);
    if (res != FR_OK) {
        // create empty easyw600.py
        f_open(&vfs_fat->fatfs, &fp, "/easyw600.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_easyw600_py, sizeof(fresh_easyw600_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }
}
#endif

static inline void *get_sp(void) {
    void *sp;
    asm volatile ("mov %0, sp;" : "=r" (sp));
    return sp;
}

static void mpy_task(void *param) {
    volatile uint32_t sp = (uint32_t)get_sp();
#if MICROPY_PY_THREAD
    mp_thread_init(&mpy_task_stk[0], MPY_TASK_SIZE * sizeof(OS_STK));
#endif
    uart_init();
    // Allocate the uPy heap using malloc and get the largest available region
    u32 mp_task_heap_size = tls_mem_get_avail_heapsize()  / 2;// * 2 / 3;
    //wm_printf("mp_task_heap_size = %u\r\n", mp_task_heap_size);
    void *mp_task_heap = tls_mem_alloc(mp_task_heap_size);

soft_reset:
    // initialise the stack pointer for the main thread
    mp_stack_set_top((void *)sp);
    mp_stack_set_limit(MPY_TASK_SIZE * sizeof(OS_STK));
    gc_init(mp_task_heap, mp_task_heap + mp_task_heap_size);
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    mp_obj_list_init(mp_sys_argv, 0);
    readline_init0();

    machine_pins_init();

#if MICROPY_USE_INTERVAL_FLS_FS
    init_spiflash_fs();
#endif

#if 1
    // run boot-up scripts
    pyexec_frozen_module("_boot.py");
    pyexec_file("boot.py");
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
        pyexec_file("main.py");
    }
#endif

    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

#if MICROPY_PY_THREAD
    mp_thread_deinit();
#endif

    mp_hal_stdout_tx_str("w600: soft reboot\r\n");

    mp_deinit();

    goto soft_reset;
}

void UserMain(void) {
    tls_os_task_create(NULL, "w600_mpy_task", mpy_task, NULL,
                       (void *)mpy_task_stk, MPY_TASK_SIZE * sizeof(OS_STK),
                       MPY_TASK_PRIO, 0);
}

void nlr_jump_fail(void *val) {
    tls_sys_reset();
}

