#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "lexer.h"
#include "lexermemzip.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "repl.h"
#include "usb.h"
#include "gc.h"
#include "led.h"

#include "Arduino.h"

extern uint32_t _heap_start;

bool do_file(const char *filename);

void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(PYB_LED_BUILTIN, 1);
        delay(250);
        led_state(PYB_LED_BUILTIN, 0);
        delay(250);
    }
}

static const char *help_text =
"Welcome to Micro Python!\n\n"
"This is a *very* early version of Micro Python and has minimal functionality.\n\n"
"Specific commands for the board:\n"
"    pyb.info()             -- print some general information\n"
"    pyb.gc()               -- run the garbage collector\n"
"    pyb.delay(<n>)         -- wait for n milliseconds\n"
"    pyb.Led(<n>)           -- create Led object for LED n (n=0)\n"
"                              Led methods: on(), off()\n"
"    pyb.gpio(<pin>)        -- read gpio pin\n"
"    pyb.gpio(<pin>, <val>) -- set gpio pin\n"
#if 0
"    pyb.Servo(<n>) -- create Servo object for servo n (n=1,2,3,4)\n"
"                      Servo methods: angle(<x>)\n"
"    pyb.switch()   -- return True/False if switch pressed or not\n"
"    pyb.accel()    -- get accelerometer values\n"
"    pyb.rand()     -- get a 16-bit random number\n"
#endif
;

// get some help about available functions
static mp_obj_t pyb_help(void) {
    printf("%s", help_text);
    return mp_const_none;
}

// get lots of info about the board
static mp_obj_t pyb_info(void) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)0x40048058;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    // get and print clock speeds
    printf("CPU=%u\nBUS=%u\nMEM=%u\n", F_CPU, F_BUS, F_MEM);

    // to print info about memory
    {
        extern void *_sdata;
        extern void *_edata;
        extern void *_sbss;
        extern void *_ebss;
        extern void *_estack;
        extern void *_etext;
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
        printf("  %lu total\n", info.total);
        printf("  %lu used %lu free\n", info.used, info.free);
        printf("  1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
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

#define RAM_START (0x1FFF8000) // fixed for chip
#define HEAP_END  (0x20006000) // tunable
#define RAM_END   (0x20008000) // fixed for chip

void gc_helper_get_regs_and_clean_stack(machine_uint_t *regs, machine_uint_t heap_end);

void gc_collect(void) {
    uint32_t start = micros();
    gc_collect_start();
    gc_collect_root((void**)RAM_START, (((uint32_t)&_heap_start) - RAM_START) / 4);
    machine_uint_t regs[10];
    gc_helper_get_regs_and_clean_stack(regs, HEAP_END);
    gc_collect_root((void**)HEAP_END, (RAM_END - HEAP_END) / 4); // will trace regs since they now live in this function on the stack
    gc_collect_end();
    uint32_t ticks = micros() - start; // TODO implement a function that does this properly

    if (0) {
        // print GC info
        gc_info_t info;
        gc_info(&info);
        printf("GC@%lu %luus\n", start, ticks);
        printf(" %lu total\n", info.total);
        printf(" %lu used %lu free\n", info.used, info.free);
        printf(" 1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
    }
}

mp_obj_t pyb_gc(void) {
    gc_collect();
    return mp_const_none;
}

mp_obj_t pyb_gpio(int n_args, mp_obj_t *args) {
    //assert(1 <= n_args && n_args <= 2);

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
    digitalWrite(pin, rt_is_true(args[1]));
    return mp_const_none;

pin_error:
    nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_ValueError, "pin %d does not exist", (void *)(machine_uint_t)pin));
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

static qstr pyb_config_source_dir = 0;
static qstr pyb_config_main = 0;

mp_obj_t pyb_source_dir(mp_obj_t source_dir) {
    pyb_config_source_dir = mp_obj_get_qstr(source_dir);
    return mp_const_none;
}

mp_obj_t pyb_main(mp_obj_t main) {
    pyb_config_main = mp_obj_get_qstr(main);
    return mp_const_none;
}

mp_obj_t pyb_delay(mp_obj_t count) {
    delay(mp_obj_get_int(count));
    return mp_const_none;
}

mp_obj_t pyb_led(mp_obj_t state) {
    led_state(PYB_LED_BUILTIN, rt_is_true(state));
    return state;
}

mp_obj_t pyb_run(mp_obj_t filename_obj) {
    const char *filename = qstr_str(mp_obj_get_qstr(filename_obj));
    do_file(filename);
    return mp_const_none;
}

char *strdup(const char *str) {
    uint32_t len = strlen(str);
    char *s2 = m_new(char, len + 1);
    memcpy(s2, str, len);
    s2[len] = 0;
    return s2;
}

#define READLINE_HIST_SIZE (8)

static const char *readline_hist[READLINE_HIST_SIZE] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void stdout_tx_str(const char *str) {
//    usart_tx_str(str);
    usb_vcp_send_str(str);
}

int readline(vstr_t *line, const char *prompt) {
    stdout_tx_str(prompt);
    int len = vstr_len(line);
    int escape = 0;
    int hist_num = 0;
    for (;;) {
        char c;
        for (;;) {
            if (usb_vcp_rx_any() != 0) {
                c = usb_vcp_rx_get();
                break;
#if 0
            } else if (usart_rx_any()) {
                c = usart_rx_char();
                break;
#endif
            }
            //delay(1);
            //if (storage_needs_flush()) {
            //    storage_flush();
            //}
        }
        if (escape == 0) {
            if (c == 4 && vstr_len(line) == len) {
                return 0;
            } else if (c == '\r') {
                stdout_tx_str("\r\n");
                for (int i = READLINE_HIST_SIZE - 1; i > 0; i--) {
                    readline_hist[i] = readline_hist[i - 1];
                }
                readline_hist[0] = strdup(vstr_str(line));
                return 1;
            } else if (c == 27) {
                escape = true;
            } else if (c == 127) {
                if (vstr_len(line) > len) {
                    vstr_cut_tail(line, 1);
                    stdout_tx_str("\b \b");
                }
            } else if (32 <= c && c <= 126) {
                vstr_add_char(line, c);
                stdout_tx_str(line->buf + line->len - 1);
            }
        } else if (escape == 1) {
            if (c == '[') {
                escape = 2;
            } else {
                escape = 0;
            }
        } else if (escape == 2) {
            escape = 0;
            if (c == 'A') {
                // up arrow
                if (hist_num < READLINE_HIST_SIZE && readline_hist[hist_num] != NULL) {
                    // erase line
                    for (int i = line->len - len; i > 0; i--) {
                        stdout_tx_str("\b \b");
                    }
                    // set line to history
                    line->len = len;
                    vstr_add_str(line, readline_hist[hist_num]);
                    // draw line
                    stdout_tx_str(readline_hist[hist_num]);
                    // increase hist num
                    hist_num += 1;
                }
            }
        } else {
            escape = 0;
        }
        delay(10);
    }
}

bool do_file(const char *filename) {
    mp_lexer_t *lex = mp_lexer_new_from_memzip_file(filename);

    if (lex == NULL) {
        printf("could not open file '%s' for reading\n", filename);
        return false;
    }

    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT);
    mp_lexer_free(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        return false;
    }

    mp_obj_t module_fun = mp_compile(pn, false);
    if (module_fun == mp_const_none) {
        return false;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        rt_call_function_0(module_fun);
        nlr_pop();
        return true;
    } else {
        // uncaught exception
        mp_obj_print((mp_obj_t)nlr.ret_val);
        printf("\n");
        return false;
    }
}

void do_repl(void) {
    stdout_tx_str("Micro Python for Teensy 3.1\r\n");
    stdout_tx_str("Type \"help()\" for more information.\r\n");

    vstr_t line;
    vstr_init(&line);

    for (;;) {
        vstr_reset(&line);
        int ret = readline(&line, ">>> ");
        if (ret == 0) {
            // EOF
            break;
        }

        if (vstr_len(&line) == 0) {
            continue;
        }

        if (mp_repl_is_compound_stmt(vstr_str(&line))) {
            for (;;) {
                vstr_add_char(&line, '\n');
                int len = vstr_len(&line);
                int ret = readline(&line, "... ");
                if (ret == 0 || vstr_len(&line) == len) {
                    // done entering compound statement
                    break;
                }
            }
        }

        mp_lexer_t *lex = mp_lexer_new_from_str_len("<stdin>", vstr_str(&line), vstr_len(&line), 0);
        mp_parse_node_t pn = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
        mp_lexer_free(lex);

        if (pn != MP_PARSE_NODE_NULL) {
            mp_obj_t module_fun = mp_compile(pn, true);
            if (module_fun != mp_const_none) {
                nlr_buf_t nlr;
                uint32_t start = micros();
                if (nlr_push(&nlr) == 0) {
                    rt_call_function_0(module_fun);
                    nlr_pop();
                    // optional timing
                    if (0) {
                        uint32_t ticks = micros() - start; // TODO implement a function that does this properly
                        printf("(took %lu us)\n", ticks);
                    }
                } else {
                    // uncaught exception
                    mp_obj_print((mp_obj_t)nlr.ret_val);
                    printf("\n");
                }
            }
        }
    }

    stdout_tx_str("\r\n");
}

int main(void) {
    pinMode(LED_BUILTIN, OUTPUT);    
    // Wait for host side to get connected
    while (!usb_vcp_is_connected()) {
        ;
    }

    led_init();
    led_state(PYB_LED_BUILTIN, 1);

//    int first_soft_reset = true;

soft_reset:

    // GC init
    gc_init(&_heap_start, (void*)HEAP_END);

    qstr_init();
    rt_init();

#if 1
    // add some functions to the python namespace
    {
        rt_store_name(qstr_from_str_static("help"), rt_make_function_0(pyb_help));
        mp_obj_t m = mp_obj_new_module(qstr_from_str_static("pyb"));
        rt_store_attr(m, qstr_from_str_static("info"), rt_make_function_0(pyb_info));
        rt_store_attr(m, qstr_from_str_static("source_dir"), rt_make_function_1(pyb_source_dir));
        rt_store_attr(m, qstr_from_str_static("main"), rt_make_function_1(pyb_main));
        rt_store_attr(m, qstr_from_str_static("gc"), rt_make_function_0(pyb_gc));
        rt_store_attr(m, qstr_from_str_static("delay"), rt_make_function_1(pyb_delay));
        rt_store_attr(m, qstr_from_str_static("led"), rt_make_function_1(pyb_led));
        rt_store_attr(m, qstr_from_str_static("Led"), rt_make_function_1(pyb_Led));
        rt_store_attr(m, qstr_from_str_static("gpio"), (mp_obj_t)&pyb_gpio_obj);
        rt_store_name(qstr_from_str_static("pyb"), m);
        rt_store_name(qstr_from_str_static("run"), rt_make_function_1(pyb_run));
    }
#endif

    if (!do_file("/boot.py")) {
        printf("Unable to open '/boot.py'\n");
        flash_error(4);
    }

    // Turn bootup LED off
    led_state(PYB_LED_BUILTIN, 0);

    // run main script
    {
        vstr_t *vstr = vstr_new();
        vstr_add_str(vstr, "/");
        if (pyb_config_source_dir == 0) {
            vstr_add_str(vstr, "src");
        } else {
            vstr_add_str(vstr, qstr_str(pyb_config_source_dir));
        }
        vstr_add_char(vstr, '/');
        if (pyb_config_main == 0) {
            vstr_add_str(vstr, "main.py");
        } else {
            vstr_add_str(vstr, qstr_str(pyb_config_main));
        }
        if (!do_file(vstr_str(vstr))) {
            printf("Unable to open '%s'\n", vstr_str(vstr));
            flash_error(3);
        }
        vstr_free(vstr);
    }

    do_repl();

    printf("PYB: soft reboot\n");

//    first_soft_reset = false;
    goto soft_reset;
}

double __aeabi_f2d(float x) {
    // TODO
    return 0.0;
}

float __aeabi_d2f(double x) {
    // TODO
    return 0.0;
}

double sqrt(double x) {
    // TODO
    return 0.0;
}

machine_float_t machine_sqrt(machine_float_t x) {
    // TODO
    return x;
}

// stub out __libc_init_array. It's called by mk20dx128.c and is used to call
// global C++ constructors. Since this is a C-only projects, we don't need to
// call constructors.
void __libc_init_array(void) {
}

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
