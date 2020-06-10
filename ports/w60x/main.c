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
#include <string.h>
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
#if MICROPY_VFS_FAT
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#endif
#if MICROPY_VFS_LFS2
#include "extmod/vfs_lfs.h"
#include "lib/littlefs/lfs2.h"
#include "mp_vfs_lfs.h"
#endif

#include "mpthreadport.h"

OS_STK mpy_task_stk[MPY_TASK_SIZE];

#if MICROPY_USE_INTERVAL_FLS_FS
fs_user_mount_t *spi_fls_vfs = NULL;
#endif

static const char fresh_boot_py[] = "# boot.py -- run on boot-up\r\n"
                                    "# can run arbitrary Python, but best to keep it minimal\r\n"
                                    "print(\"\")\r\n"
                                    "print(\"    WinnerMicro W600\")\r\n"
                                    "print(\"\")\r\n";
static const char fresh_main_py[] = "# main.py -- put your code here!\r\n";

#if MICROPY_USE_INTERVAL_FLS_FS
static void init_spiflash_fs (void) {
    // Initialise the local flash filesystem.
    // init the vfs object
    spi_fls_vfs = m_new_obj_maybe(fs_user_mount_t);
    if (!spi_fls_vfs) {
        printf("failed to init filesystem\r\n");
        return;
    }

    fs_user_mount_t *vfs_fs = spi_fls_vfs;

#if MICROPY_VFS_FAT
    FILINFO fno;
    FIL fp;
#endif
#if MICROPY_VFS_LFS2
    struct lfs2_info fno;
    lfs2_file_t fp;
#endif

    w600_flash_init_vfs(vfs_fs);

#if MICROPY_VFS_FAT
    // Create it if needed, and mount in on /flash.
    FRESULT res = f_mount(&vfs_fs->fatfs);
    if (res == FR_NO_FILESYSTEM) {
        // no filesystem, so create a fresh one
        uint8_t working_buf[FF_MAX_SS];
        res = f_mkfs(&vfs_fs->fatfs, FM_FAT, 0, working_buf, sizeof(working_buf));
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
        printf("failed to mount flash\r\n");
        return;
    }
#endif
#if MICROPY_VFS_LFS2
    // Create it if needed, and mount in on /flash.
    int res = lfs2_mount(&vfs_fs->lfs, &vfs_fs->config);
    if (res != LFS2_ERR_OK) {
        res = lfs2_format(&vfs_fs->lfs, &vfs_fs->config);
        if (res == LFS2_ERR_OK) {
            // success creating fresh LFS
            res = lfs2_mount(&vfs_fs->lfs, &vfs_fs->config);
            if (res != LFS2_ERR_OK) {
                printf("failed to mount flash\r\n");
                return;
            }
        } else {
            printf("failed to create filesystem, err = %d\r\n", res);
            return;
        }

    }
#endif

    // mount the flash device (there should be no other devices mounted at this point)
    // we allocate this structure on the heap because vfs->next is a root pointer
    mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
    if (vfs == NULL) {
        printf("failed to init filesystem: no mem\r\n");
        return;
    }
    vfs->str = "/flash";
    vfs->len = 6;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fs);
    vfs->next = NULL;

    MP_STATE_VM(vfs_mount_table) = vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = vfs;

#if MICROPY_VFS_FAT

    f_chdir(&vfs_fs->fatfs, "/");

    // Make sure we have a /flash/boot.py.  Create it if needed.
    res = f_stat(&vfs_fs->fatfs, "/boot.py", &fno);
    if (res != FR_OK) { // doesn't exist, create fresh file
        FIL fp;
        f_open(&vfs_fs->fatfs, &fp, "/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }

    res = f_stat(&vfs_fs->fatfs, "/main.py", &fno);
    if (res != FR_OK) {
        // create empty main.py
        f_open(&vfs_fs->fatfs, &fp, "/main.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
    }
#endif
#if MICROPY_VFS_LFS2
    /* In the LittleFS file system, 
       each directory occupies at least 2 sectors, 
       and each file occupies at least 1 sector. 
    */

    //Initialize the current working directory (cwd)
    vstr_init(&(vfs_fs->cur_dir), 16);
    vstr_add_str(&(vfs_fs->cur_dir), "/");

    // Make sure we have a /flash/boot.py.  Create it if needed.
    res = lfs2_stat(&vfs_fs->lfs, "/boot.py", &fno);
    if (res != LFS2_ERR_OK) { // doesn't exist, create fresh file
        lfs2_file_open(&vfs_fs->lfs, &fp, "/boot.py", LFS2_O_WRONLY | LFS2_O_CREAT);
        lfs2_file_write(&vfs_fs->lfs, &fp, fresh_boot_py, sizeof(fresh_boot_py) - 1);
        // TODO check we could write bytes
        lfs2_file_close(&vfs_fs->lfs, &fp);
    }

    res = lfs2_stat(&vfs_fs->lfs, "/main.py", &fno);
    if (res != LFS2_ERR_OK) {
        // create empty main.py
        lfs2_file_open(&vfs_fs->lfs, &fp, "/main.py", LFS2_O_WRONLY | LFS2_O_CREAT);
        lfs2_file_write(&vfs_fs->lfs, &fp, fresh_main_py, sizeof(fresh_main_py) - 1);
        // TODO check we could write bytes
        lfs2_file_close(&vfs_fs->lfs, &fp);
    }
#endif
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
    mp_thread_init(mpy_task_stk, MPY_TASK_SIZE);
#endif

    uart_init();

    // Allocate the uPy heap using malloc and get the largest available region
#ifdef W60X_USE_MBEDTLS_SSL
    u32 mp_task_heap_size = 36 * 1024;
#else
    u32 mp_task_heap_size = tls_mem_get_avail_heapsize() / 2;
#endif
    //printf("mp_task_heap_size = %u\r\n", mp_task_heap_size);

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
    timer_init0();

    machine_pins_init();

#if MICROPY_USE_INTERVAL_FLS_FS
    init_spiflash_fs();
#endif

#if MICROPY_USE_FROZEN_SCRIPT
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
    tls_os_task_create(NULL, "w60xmpy", mpy_task, NULL,
                       (void *)mpy_task_stk, sizeof(mpy_task_stk),
                       MPY_TASK_PRIO, 0);
}

void nlr_jump_fail(void *val) {
    tls_sys_reset();
    for (;;); // Just to silence the compiler warning
}

