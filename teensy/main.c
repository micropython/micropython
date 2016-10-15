#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/nlr.h"
#include "py/parse.h"
#include "py/lexer.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "gccollect.h"
#include "lib/utils/pyexec.h"
#include "readline.h"
#include "lexermemzip.h"

#include "Arduino.h"

#include "servo.h"
#include "led.h"
#include "uart.h"
#include "pin.h"
#include "lib/fatfs/ff.h"
#include "extmod/fsusermount.h"
#include "sdcard.h"
#include "modmachine.h"

extern uint32_t _heap_start;

void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(PYB_LED_BUILTIN, 1);
        delay(250);
        led_state(PYB_LED_BUILTIN, 0);
        delay(250);
    }
}

void NORETURN __fatal_error(const char *msg) {
    for (volatile uint delay = 0; delay < 10000000; delay++) {
    }
    led_state(1, 1);
    led_state(2, 1);
    led_state(3, 1);
    led_state(4, 1);
    mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 14);
    mp_hal_stdout_tx_strn(msg, strlen(msg));
    for (uint i = 0;;) {
        led_toggle(((i++) & 3) + 1);
        for (volatile uint delay = 0; delay < 10000000; delay++) {
        }
        if (i >= 16) {
            // to conserve power
            __WFI();
        }
    }
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    __fatal_error("");
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {

    printf("Assertion failed: %s, file %s, line %d\n", expr, file, line);
    __fatal_error("");
}

mp_obj_t pyb_analog_read(mp_obj_t pin_obj) {
    uint pin = mp_obj_get_int(pin_obj);
    int val = analogRead(pin);
    return MP_OBJ_NEW_SMALL_INT(val);
}

mp_obj_t pyb_analog_write(mp_obj_t pin_obj, mp_obj_t val_obj) {
    uint pin = mp_obj_get_int(pin_obj);
    int val = mp_obj_get_int(val_obj);
    analogWrite(pin, val);
    return mp_const_none;
}

mp_obj_t pyb_analog_write_resolution(mp_obj_t res_obj) {
    int res = mp_obj_get_int(res_obj);
    analogWriteResolution(res);
    return mp_const_none;
}

mp_obj_t pyb_analog_write_frequency(mp_obj_t pin_obj, mp_obj_t freq_obj) {
    uint pin = mp_obj_get_int(pin_obj);
    int freq = mp_obj_get_int(freq_obj);
    analogWriteFrequency(pin, freq);
    return mp_const_none;
}

STATIC mp_obj_t pyb_config_source_dir = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_main = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_usb_mode = MP_OBJ_NULL;

mp_obj_t pyb_source_dir(mp_obj_t source_dir) {
    if (MP_OBJ_IS_STR(source_dir)) {
        pyb_config_source_dir = source_dir;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_source_dir_obj, pyb_source_dir);

mp_obj_t pyb_main(mp_obj_t main) {
    if (MP_OBJ_IS_STR(main)) {
        pyb_config_main = main;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_main_obj, pyb_main);

STATIC mp_obj_t pyb_usb_mode(mp_obj_t usb_mode) {
    if (MP_OBJ_IS_STR(usb_mode)) {
        pyb_config_usb_mode = usb_mode;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_mode_obj, pyb_usb_mode);

int main(void) {
    // TODO: Put this in a more common initialization function.
    // Turn on STKALIGN which keeps the stack 8-byte aligned for interrupts
    // (per EABI)
    #define SCB_CCR_STKALIGN (1 << 9)
    SCB_CCR |= SCB_CCR_STKALIGN;

    mp_stack_ctrl_init();
    mp_stack_set_limit((char*)&_ram_end - (char*)&_heap_end - 1024);

    pinMode(LED_BUILTIN, OUTPUT);
    led_init();

soft_reset:

    led_state(PYB_LED_BUILTIN, 1);

#if MICROPY_HW_HAS_SDCARD
    sdcard_init();
#endif

    // GC init
    gc_init(&_heap_start, &_heap_end);

    // Micro Python init
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_init(mp_sys_argv, 0);

    readline_init0();

    pin_init0();

#if MICROPY_HW_HAS_SDCARD
    // if an SD card is present then mount it on /sd/
    if (sdcard_is_present()) {
        // create vfs object
        fs_user_mount_t *vfs = m_new_obj_maybe(fs_user_mount_t);
        if (vfs == NULL) {
            goto no_mem_for_sd;
        }
        vfs->str = "/sd";
        vfs->len = 3;
        vfs->flags = FSUSER_FREE_OBJ;
        sdcard_init_vfs(vfs);

        // put the sd device in slot 1 (it will be unused at this point)
        MP_STATE_PORT(fs_user_mount)[1] = vfs;

        FRESULT res = f_mount(&vfs->fatfs, vfs->str, 1);
        if (res != FR_OK) {
            printf("PYB: can't mount SD card\n");
            MP_STATE_PORT(fs_user_mount)[1] = NULL;
            m_del_obj(fs_user_mount_t, vfs);
        } else {
            mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd));
            mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd_slash_lib));
            // use SD card as current directory
            f_chdrive("/sd");
        }
        no_mem_for_sd:;
    }
#endif

#if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("boot.py");
#else
    if (!pyexec_file("/boot.py")) {
        flash_error(4);
    }
#endif

    // Turn bootup LED off
    led_state(PYB_LED_BUILTIN, 0);

    // run main script
#if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("main.py");
#else
    {
        vstr_t *vstr = vstr_new(16);
        vstr_add_str(vstr, "/");
        if (pyb_config_main == MP_OBJ_NULL) {
            vstr_add_str(vstr, "main.py");
        } else {
            vstr_add_str(vstr, mp_obj_str_get_str(pyb_config_main));
        }
        if (!pyexec_file(vstr_null_terminated_str(vstr))) {
            flash_error(3);
        }
        vstr_free(vstr);
    }
#endif

    // enter REPL
    // REPL mode can change, or it can request a soft reset
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

    printf("PYB: soft reboot\n");

    machine_soft_reset = true;
    goto soft_reset;
}

// stub out __libc_init_array. It's called by mk20dx128.c and is used to call
// global C++ constructors. Since this is a C-only projects, we don't need to
// call constructors.
void __libc_init_array(void) {
}

// ultoa is used by usb_init_serialnumber. Normally ultoa would be provided
// by nonstd.c from the teensy core, but it conflicts with some of the
// MicroPython functions in string0.c, so we provide ultoa here.
char * ultoa(unsigned long val, char *buf, int radix) 	
{
	unsigned digit;
	int i=0, j;
	char t;

	while (1) {
		digit = val % radix;
		buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
		val /= radix;
		if (val == 0) break;
		i++;
	}
	buf[i + 1] = 0;
	for (j=0; j < i; j++, i--) {
		t = buf[j];
		buf[j] = buf[i];
		buf[i] = t;
	}
	return buf;
}
