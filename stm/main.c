#include <stdio.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm_misc.h>
#include "std.h"

#include "misc.h"
#include "systick.h"
#include "led.h"
#include "lcd.h"
#include "storage.h"
#include "mma.h"
#include "usb.h"
#include "ff.h"

static void impl02_c_version() {
    int x = 0;
    while (x < 400) {
        int y = 0;
        while (y < 400) {
            volatile int z = 0;
            while (z < 400) {
                z = z + 1;
            }
            y = y + 1;
        }
        x = x + 1;
    }
}

#define PYB_USRSW_PORT (GPIOA)
#define PYB_USRSW_PIN (GPIO_Pin_13)

void sw_init() {
    // make it an input with pull-up
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PYB_USRSW_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(PYB_USRSW_PORT, &GPIO_InitStructure);
}

int sw_get() {
    if (PYB_USRSW_PORT->IDR & PYB_USRSW_PIN) {
        // pulled high, so switch is not pressed
        return 0;
    } else {
        // pulled low, so switch is pressed
        return 1;
    }
}

void __fatal_error(const char *msg) {
    lcd_print_strn("\nFATAL ERROR:\n", 14);
    lcd_print_strn(msg, strlen(msg));

    for (;;) {
        led_state(PYB_LED_R1, 1);
        led_state(PYB_LED_R2, 0);
        sys_tick_delay_ms(150);
        led_state(PYB_LED_R1, 0);
        led_state(PYB_LED_R2, 1);
        sys_tick_delay_ms(150);
    }
}

#include "nlr.h"
#include "misc.h"
#include "lexer.h"
#include "lexerstm.h"
#include "mpyconfig.h"
#include "parse.h"
#include "compile.h"
#include "runtime.h"

py_obj_t pyb_delay(py_obj_t count) {
    sys_tick_delay_ms(rt_get_int(count));
    return py_const_none;
}

py_obj_t pyb_led(py_obj_t state) {
    led_state(PYB_LED_G1, rt_is_true(state));
    return state;
}

py_obj_t pyb_sw() {
    if (sw_get()) {
        return py_const_true;
    } else {
        return py_const_false;
    }
}

FATFS fatfs0;


/*
void g(uint i) {
    printf("g:%d\n", i);
    if (i & 1) {
        nlr_jump((void*)(42 + i));
    }
}
void f() {
    nlr_buf_t nlr;
    int i;
    for (i = 0; i < 4; i++) {
        printf("f:loop:%d:%p\n", i, &nlr);
        if (nlr_push(&nlr) == 0) {
            // normal
            //printf("a:%p:%p %p %p %u\n", &nlr, nlr.ip, nlr.sp, nlr.prev, nlr.ret_val);
            g(i);
            printf("f:lp:%d:nrm\n", i);
            nlr_pop();
        } else {
            // nlr
            //printf("b:%p:%p %p %p %u\n", &nlr, nlr.ip, nlr.sp, nlr.prev, nlr.ret_val);
            printf("f:lp:%d:nlr:%d\n", i, (int)nlr.ret_val);
        }
    }
}
void nlr_test() {
    f(1);
}
*/

void fatality() {
    led_state(PYB_LED_R1, 1);
    led_state(PYB_LED_G1, 1);
    led_state(PYB_LED_R2, 1);
    led_state(PYB_LED_G2, 1);
}

static const char fresh_boot_py[] =
"# boot.py -- run on boot-up\n"
"# can run arbitrary Python, but best to keep it minimal\n"
"\n"
"pyb.source_dir('/src')\n"
"pyb.main('main.py')\n"
"#pyb.usb_usr('VCP')\n"
"#pyb.usb_msd(True, 'dual partition')\n"
"#pyb.flush_cache(False)\n"
"#pyb.error_log('error.txt')\n"
;

// get lots of info about the board
static void board_info() {
    // get and print clock speeds
    // SYSCLK=168MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
    {
        RCC_ClocksTypeDef rcc_clocks;
        RCC_GetClocksFreq(&rcc_clocks);
        printf("S=%lu\nH=%lu\nP1=%lu\nP2=%lu\n", rcc_clocks.SYSCLK_Frequency, rcc_clocks.HCLK_Frequency, rcc_clocks.PCLK1_Frequency, rcc_clocks.PCLK2_Frequency);
    }

    // to print info about memory
    {
        extern void *_sidata;
        extern void *_sdata;
        extern void *_edata;
        extern void *_sbss;
        extern void *_ebss;
        extern void *_estack;
        extern void *_etext;
        extern void *_heap_start;
        printf("_sidata=%p\n", &_sidata);
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_estack=%p\n", &_estack);
        printf("_etext=%p\n", &_etext);
        printf("_heap_start=%p\n", &_heap_start);
    }

    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("0:", &nclst, &fatfs);
        printf("free=%u\n", (uint)(nclst * fatfs->csize * 512));
    }
}

char *readline(const char *prompt) {
    vstr_t vstr;
    vstr_init(&vstr);
    usb_vcp_send_str(prompt);
    for (;;) {
        //extern int rx_buf_in;
        //extern int rx_buf_out;
        while (usb_vcp_rx_any() == 0) {
            //printf("nope %x %x\n", rx_buf_in, rx_buf_out);
            sys_tick_delay_ms(10);
        }
        char c = usb_vcp_rx_get();
        if (c == 4 && vstr_len(&vstr) == 0) {
            return NULL;
        } else if (c == '\r') {
            usb_vcp_send_str("\r\n");
            return vstr_str(&vstr);
        } else if (c == 127) {
            if (vstr_len(&vstr) > 0) {
                vstr_cut_tail(&vstr, 1);
                usb_vcp_send_str("\b \b");
            }
        } else if (32 <= c && c <= 126) {
            vstr_add_char(&vstr, c);
            usb_vcp_send_strn(&c, 1);
        }
        sys_tick_delay_ms(100);
    }
    return NULL;
}

void do_repl() {
    usb_vcp_send_str("Micro Python\r\n");

    for (;;) {
        char *line = readline(">>> ");
        if (line == NULL) {
            // EOF
            return;
        }
        /*
        if (is_compound_stmt(line)) {
            for (;;) {
                char *line2 = readline("... ");
                if (line2 == NULL || strlen(line2) == 0) {
                    break;
                }
                char *line3 = str_join(line, '\n', line2);
                m_free(line);
                m_free(line2);
                line = line3;
            }
        }
        */

        py_lexer_str_buf_t sb;
        py_lexer_t *lex = py_lexer_new_from_str_len("<stdin>", line, strlen(line), false, &sb);
        py_parse_node_t pn = py_parse(lex, PY_PARSE_SINGLE_INPUT);
        py_lexer_free(lex);

        if (pn != PY_PARSE_NODE_NULL) {
            bool comp_ok = py_compile(pn, true);
            if (comp_ok) {
                py_obj_t module_fun = rt_make_function_from_id(1);
                if (module_fun != py_const_none) {
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        rt_call_function_0(module_fun);
                        nlr_pop();
                    } else {
                        // uncaught exception
                        py_obj_print((py_obj_t)nlr.ret_val);
                        printf("\n");
                    }
                }
            }
        }
    }
}

int main() {
    // TODO disable JTAG

    // set interrupt priority config to use all 4 bits for pre-empting
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // enable the CCM RAM and the GPIO's
    RCC->AHB1ENR |= RCC_AHB1ENR_CCMDATARAMEN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // basic sub-system init
    sys_tick_init();
    led_init();

    // turn on LED to indicate bootup
    led_state(PYB_LED_G1, 1);

    // more sub-system init
    sw_init();
    lcd_init();
    storage_init();

    // Python init
    qstr_init();
    rt_init();

    // add some functions to the python namespace
    rt_store_name(qstr_from_str_static("pyb_delay"), rt_make_function_1(pyb_delay));
    rt_store_name(qstr_from_str_static("pyb_led"), rt_make_function_1(pyb_led));
    rt_store_name(qstr_from_str_static("pyb_sw"), rt_make_function_0(pyb_sw));

    // print a message
    printf(" micro py board\n");

    // local filesystem init
    {
        // try to mount the flash
        FRESULT res = f_mount(&fatfs0, "0:", 1);
        if (res == FR_OK) {
            // mount sucessful
        } else if (res == FR_NO_FILESYSTEM) {
            // no filesystem, so create a fresh one

            // LED on to indicate creation of LFS
            led_state(PYB_LED_R2, 1);
            uint32_t stc = sys_tick_counter;

            res = f_mkfs("0:", 0, 0);
            if (res == FR_OK) {
                // success creating fresh LFS
            } else {
                __fatal_error("could not create LFS");
            }

            // keep LED on for at least 200ms
            sys_tick_wait_at_least(stc, 200);
            led_state(PYB_LED_R2, 0);
        } else {
            __fatal_error("could not access LFS");
        }
    }

    // make sure we have a /boot.py
    {
        FILINFO fno;
        FRESULT res = f_stat("0:/boot.py", &fno);
        if (res == FR_OK) {
            if (fno.fattrib & AM_DIR) {
                // exists as a directory
                // TODO handle this case
                // see http://elm-chan.org/fsw/ff/img/app2.c for a "rm -rf" implementation
            } else {
                // exists as a file, good!
            }
        } else {
            // doesn't exist, create fresh file

            // LED on to indicate creation of boot.py
            led_state(PYB_LED_R2, 1);
            uint32_t stc = sys_tick_counter;

            FIL fp;
            f_open(&fp, "0:/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
            UINT n;
            f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py), &n);
            // TODO check we could write n bytes
            f_close(&fp);

            // keep LED on for at least 200ms
            sys_tick_wait_at_least(stc, 200);
            led_state(PYB_LED_R2, 0);
        }
    }

    // run /boot.py
    if (0) {
        FIL fp;
        f_open(&fp, "0:/boot.py", FA_READ);
        UINT n;
        char buf[20];
        f_read(&fp, buf, 18, &n);
        buf[n + 1] = 0;
        printf("read %d\n%s", n, buf);
        f_close(&fp);
    }

    // turn boot-up LED off
    led_state(PYB_LED_G1, 0);

    // USB
    if (1) {
        usb_init();
    }

    //printf("init;al=%u\n", m_get_total_bytes_allocated()); // 1600, due to qstr_init
    //sys_tick_delay_ms(1000);

    // Python!
    if (0) {
        //const char *pysrc = "def f():\n  x=x+1\nprint(42)\n";
        const char *pysrc =
            // impl01.py
            /*
            "x = 0\n"
            "while x < 400:\n"
            "    y = 0\n"
            "    while y < 400:\n"
            "        z = 0\n"
            "        while z < 400:\n"
            "            z = z + 1\n"
            "        y = y + 1\n"
            "    x = x + 1\n";
            */
            // impl02.py
            /*
            "#@micropython.native\n"
            "def f():\n"
            "    x = 0\n"
            "    while x < 400:\n"
            "        y = 0\n"
            "        while y < 400:\n"
            "            z = 0\n"
            "            while z < 400:\n"
            "                z = z + 1\n"
            "            y = y + 1\n"
            "        x = x + 1\n"
            "f()\n";
            */
            "print('in python!')\n"
            "x = 0\n"
            "while x < 4:\n"
            "    pyb_led(True)\n"
            "    pyb_delay(201)\n"
            "    pyb_led(False)\n"
            "    pyb_delay(201)\n"
            "    x += 1\n"
            "print('press me!')\n"
            "while True:\n"
            "    pyb_led(pyb_sw())\n";
            /*
            // impl16.py
            "@micropython.asm_thumb\n"
            "def delay(r0):\n"
            "    b(loop_entry)\n"
            "    label(loop1)\n"
            "    movw(r1, 55999)\n"
            "    label(loop2)\n"
            "    subs(r1, r1, 1)\n"
            "    cmp(r1, 0)\n"
            "    bgt(loop2)\n"
            "    subs(r0, r0, 1)\n"
            "    label(loop_entry)\n"
            "    cmp(r0, 0)\n"
            "    bgt(loop1)\n"
            "print('in python!')\n"
            "@micropython.native\n"
            "def flash(n):\n"
            "    x = 0\n"
            "    while x < n:\n"
            "        pyb_led(True)\n"
            "        delay(249)\n"
            "        pyb_led(False)\n"
            "        delay(249)\n"
            "        x = x + 1\n"
            "flash(20)\n";
            */
            // impl18.py
            /*
            "# basic exceptions\n"
            "x = 1\n"
            "try:\n"
            "    x.a()\n"
            "except:\n"
            "    print(x)\n";
            */
            /*
            // impl19.py
            "# for loop\n"
            "def f():\n"
            "    for x in range(400):\n"
            "        for y in range(400):\n"
            "            for z in range(400):\n"
            "                pass\n"
            "f()\n";
            */

        py_lexer_str_buf_t py_lexer_str_buf;
        py_lexer_t *lex = py_lexer_new_from_str_len("<stdin>", pysrc, strlen(pysrc), false, &py_lexer_str_buf);

        // nalloc=1740;6340;6836 -> 140;4600;496 bytes for lexer, parser, compiler
        printf("lex; al=%u\n", m_get_total_bytes_allocated());
        sys_tick_delay_ms(1000);
        py_parse_node_t pn = py_parse(lex, PY_PARSE_FILE_INPUT);
        py_lexer_free(lex);
        if (pn != PY_PARSE_NODE_NULL) {
            printf("pars;al=%u\n", m_get_total_bytes_allocated());
            sys_tick_delay_ms(1000);
            //parse_node_show(pn, 0);
            bool comp_ok = py_compile(pn, false);
            printf("comp;al=%u\n", m_get_total_bytes_allocated());
            sys_tick_delay_ms(1000);

            if (!comp_ok) {
                printf("compile error\n");
            } else {
                // execute it!

                py_obj_t module_fun = rt_make_function_from_id(1);

                // flash once
                led_state(PYB_LED_G1, 1);
                sys_tick_delay_ms(100);
                led_state(PYB_LED_G1, 0);

                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    py_obj_t ret = rt_call_function_0(module_fun);
                    printf("done! got: ");
                    py_obj_print(ret);
                    printf("\n");
                    nlr_pop();
                } else {
                    // uncaught exception
                    printf("exception: ");
                    py_obj_print((py_obj_t)nlr.ret_val);
                    printf("\n");
                }

                // flash once
                led_state(PYB_LED_G1, 1);
                sys_tick_delay_ms(100);
                led_state(PYB_LED_G1, 0);

                sys_tick_delay_ms(1000);
                printf("nalloc=%u\n", m_get_total_bytes_allocated());
                sys_tick_delay_ms(1000);
            }
        }
    }

    do_repl();

    // benchmark C version of impl02.py
    if (0) {
        led_state(PYB_LED_G1, 1);
        sys_tick_delay_ms(100);
        led_state(PYB_LED_G1, 0);
        impl02_c_version();
        led_state(PYB_LED_G1, 1);
        sys_tick_delay_ms(100);
        led_state(PYB_LED_G1, 0);
    }

    // MMA testing
    if (0) {
        printf("1");
        mma_init();
        printf("2");
        mma_start(0x4c, 1);
        printf("3");
        mma_send_byte(0);
        printf("4");
        mma_stop();
        printf("5");
        mma_start(0x4c, 1);
        printf("6");
        mma_send_byte(0);
        printf("7");
        mma_restart(0x4c, 0);
        for (int i = 0; i <= 0xa; i++) {
            int data;
            if (i == 0xa) {
                data = mma_read_nack();
            } else {
                data = mma_read_ack();
            }
            printf(" %02x", data);
        }
        printf("\n");

        mma_start(0x4c, 1);
        mma_send_byte(7); // mode
        mma_send_byte(1); // active mode
        mma_stop();

        for (;;) {
            sys_tick_delay_ms(500);

            mma_start(0x4c, 1);
            mma_send_byte(0);
            mma_restart(0x4c, 0);
            for (int i = 0; i <= 3; i++) {
                int data;
                if (i == 3) {
                    data = mma_read_nack();
                    printf(" %02x\n", data);
                } else {
                    data = mma_read_ack() & 0x3f;
                    if (data & 0x20) {
                        data |= 0xc0;
                    }
                    printf(" % 2d", data);
                }
            }
        }
    }

    // SD card testing
    if (0) {
        //sdio_init();
    }

    int i = 0;
    int n = 0;
    uint32_t stc = sys_tick_counter;

    for (;;) {
        sys_tick_delay_ms(10);
        if (sw_get()) {
            led_state(PYB_LED_G1, 1);
            i = 1 - i;
            if (i) {
                printf(" angel %05x.\n", n);
                //usb_vcp_send("hello!\r\n", 8);
            } else {
                printf(" mishka %4u.\n", n);
                //usb_vcp_send("angel!\r\n", 8);
            }
            n += 1;
        } else {
            led_state(PYB_LED_G1, 0);
        }
        if (sys_tick_has_passed(stc, 500)) {
            stc += 500;
            led_toggle(PYB_LED_G2);
        }
    }

    return 0;
}
