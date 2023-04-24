#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/lexer.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "gccollect.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "lexermemzip.h"

#include "Arduino.h"

#include "servo.h"
#include "led.h"
#include "uart.h"
#include "pin.h"

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

#if 0
// get lots of info about the board
static mp_obj_t pyb_info(void) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte *)0x40048058;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    // get and print clock speeds
    printf("CPU=%u\nBUS=%u\nMEM=%u\n", F_CPU, F_BUS, F_MEM);

    // to print info about memory
    {
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_estack=%p\n", &_estack);
        printf("_etext=%p\n", &_etext);
        printf("_heap_start=%p\n", &_heap_start);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  %u total\n", info.total);
        printf("  %u used %u free\n", info.used, info.free);
        printf("  1=%u 2=%u m=%u\n", info.num_1block, info.num_2block, info.max_block);
    }

    #if 0
    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("0:", &nclst, &fatfs);
        printf("LFS free: %u bytes\n", (uint)(nclst * fatfs->csize * 512));
    }
    #endif

    return mp_const_none;
}

#endif

#define RAM_START (0x1FFF8000) // fixed for chip
#define HEAP_END  (0x20006000) // tunable
#define RAM_END   (0x20008000) // fixed for chip

#if 0

void gc_helper_get_regs_and_clean_stack(mp_uint_t *regs, mp_uint_t heap_end);

mp_obj_t pyb_gc(void) {
    gc_collect();
    return mp_const_none;
}

mp_obj_t pyb_gpio(int n_args, mp_obj_t *args) {
    // assert(1 <= n_args && n_args <= 2);

    uint pin = mp_obj_get_int(args[0]);
    if (pin > CORE_NUM_DIGITAL) {
        goto pin_error;
    }

    if (n_args == 1) {
        // get pin
        pinMode(pin, INPUT);
        return MP_OBJ_NEW_SMALL_INT(digitalRead(pin));
    }

    // set pin
    pinMode(pin, OUTPUT);
    digitalWrite(pin, mp_obj_is_true(args[1]));
    return mp_const_none;

pin_error:
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("pin %d does not exist"), pin);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_obj, 1, 2, pyb_gpio);

#if 0
mp_obj_t pyb_hid_send_report(mp_obj_t arg) {
    mp_obj_t *items = mp_obj_get_array_fixed_n(arg, 4);
    uint8_t data[4];
    data[0] = mp_obj_get_int(items[0]);
    data[1] = mp_obj_get_int(items[1]);
    data[2] = mp_obj_get_int(items[2]);
    data[3] = mp_obj_get_int(items[3]);
    usb_hid_send_report(data);
    return mp_const_none;
}
#endif

#endif // 0

STATIC mp_obj_t pyb_config_source_dir = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_main = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_usb_mode = MP_OBJ_NULL;

mp_obj_t pyb_source_dir(mp_obj_t source_dir) {
    if (mp_obj_is_str(source_dir)) {
        pyb_config_source_dir = source_dir;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_source_dir_obj, pyb_source_dir);

mp_obj_t pyb_main(mp_obj_t main) {
    if (mp_obj_is_str(main)) {
        pyb_config_main = main;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_main_obj, pyb_main);

STATIC mp_obj_t pyb_usb_mode(mp_obj_t usb_mode) {
    if (mp_obj_is_str(usb_mode)) {
        pyb_config_usb_mode = usb_mode;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_mode_obj, pyb_usb_mode);

#if 0

mp_obj_t pyb_delay(mp_obj_t count) {
    delay(mp_obj_get_int(count));
    return mp_const_none;
}

mp_obj_t pyb_led(mp_obj_t state) {
    led_state(PYB_LED_BUILTIN, mp_obj_is_true(state));
    return state;
}

#endif  // 0

#if 0
char *strdup(const char *str) {
    uint32_t len = strlen(str);
    char *s2 = m_new(char, len + 1);
    memcpy(s2, str, len);
    s2[len] = 0;
    return s2;
}
#endif

int main(void) {
    // TODO: Put this in a more common initialization function.
    // Turn on STKALIGN which keeps the stack 8-byte aligned for interrupts
    // (per EABI)
    #define SCB_CCR_STKALIGN (1 << 9)
    SCB_CCR |= SCB_CCR_STKALIGN;

    mp_stack_ctrl_init();
    mp_stack_set_limit(10240);

    pinMode(LED_BUILTIN, OUTPUT);
    led_init();

//    int first_soft_reset = true;

soft_reset:

    led_state(PYB_LED_BUILTIN, 1);

    // GC init
    gc_init(&_heap_start, (void *)HEAP_END);

    // MicroPython init
    mp_init();

    readline_init0();

    pin_init0();

    #if 0
    // add some functions to the python namespace
    {
        mp_store_name(MP_QSTR_help, mp_make_function_n(0, pyb_help));
        mp_obj_t m = mp_obj_new_module(MP_QSTR_pyb);
        mp_store_attr(m, MP_QSTR_info, mp_make_function_n(0, pyb_info));
        mp_store_attr(m, MP_QSTR_source_dir, mp_make_function_n(1, pyb_source_dir));
        mp_store_attr(m, MP_QSTR_main, mp_make_function_n(1, pyb_main));
        mp_store_attr(m, MP_QSTR_gc, mp_make_function_n(0, pyb_gc));
        mp_store_attr(m, MP_QSTR_delay, mp_make_function_n(1, pyb_delay));
        mp_store_attr(m, MP_QSTR_led, mp_make_function_n(1, pyb_led));
        mp_store_attr(m, MP_QSTR_LED, (mp_obj_t)&pyb_led_type);
        mp_store_attr(m, MP_QSTR_analogRead, mp_make_function_n(1, pyb_analog_read));
        mp_store_attr(m, MP_QSTR_analogWrite, mp_make_function_n(2, pyb_analog_write));
        mp_store_attr(m, MP_QSTR_analogWriteResolution, mp_make_function_n(1, pyb_analog_write_resolution));
        mp_store_attr(m, MP_QSTR_analogWriteFrequency, mp_make_function_n(2, pyb_analog_write_frequency));

        mp_store_attr(m, MP_QSTR_gpio, (mp_obj_t)&pyb_gpio_obj);
        mp_store_attr(m, MP_QSTR_Servo, mp_make_function_n(0, pyb_Servo));
        mp_store_name(MP_QSTR_pyb, m);
    }
    #endif

    #if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("boot.py", false);
    #else
    if (!pyexec_file_if_exists("/boot.py")) {
        flash_error(4);
    }
    #endif

    // Turn bootup LED off
    led_state(PYB_LED_BUILTIN, 0);

    // run main script
    #if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("main.py", true);
    #else
    {
        vstr_t *vstr = vstr_new(16);
        vstr_add_str(vstr, "/");
        if (pyb_config_main == MP_OBJ_NULL) {
            vstr_add_str(vstr, "main.py");
        } else {
            vstr_add_str(vstr, mp_obj_str_get_str(pyb_config_main));
        }
        if (!pyexec_file_if_exists(vstr_null_terminated_str(vstr))) {
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

    printf("MPY: soft reboot\n");

//    first_soft_reset = false;
    goto soft_reset;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

// stub out __libc_init_array. It's called by mk20dx128.c and is used to call
// global C++ constructors. Since this is a C-only projects, we don't need to
// call constructors.
void __libc_init_array(void) {
}

// ultoa is used by usb_init_serialnumber. Normally ultoa would be provided
// by nonstd.c from the teensy core, but it conflicts with some of the
// MicroPython functions in string0.c, so we provide ultoa here.
char *ultoa(unsigned long val, char *buf, int radix) {
    unsigned digit;
    int i = 0, j;
    char t;

    while (1) {
        digit = val % radix;
        buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
        val /= radix;
        if (val == 0) {
            break;
        }
        i++;
    }
    buf[i + 1] = 0;
    for (j = 0; j < i; j++, i--) {
        t = buf[j];
        buf[j] = buf[i];
        buf[i] = t;
    }
    return buf;
}
