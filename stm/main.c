#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_pwr.h>
#include <stm32f4xx_rtc.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_rng.h>
#include <stm_misc.h>
#include "std.h"

#include "misc.h"
#include "ff.h"
#include "mpconfig.h"
#include "qstr.h"
#include "nlr.h"
#include "misc.h"
#include "lexer.h"
#include "lexerfatfs.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "repl.h"
#include "gc.h"
#include "systick.h"
#include "led.h"
#include "servo.h"
#include "lcd.h"
#include "storage.h"
#include "mma.h"
#include "usart.h"
#include "usb.h"
#include "timer.h"
#include "audio.h"
#include "pybwlan.h"
#include "i2c.h"
#include "usrsw.h"
#include "adc.h"

int errno;

extern uint32_t _heap_start;

static FATFS fatfs0;

void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(PYB_LED_R1, 1);
        led_state(PYB_LED_R2, 0);
        sys_tick_delay_ms(250);
        led_state(PYB_LED_R1, 0);
        led_state(PYB_LED_R2, 1);
        sys_tick_delay_ms(250);
    }
    led_state(PYB_LED_R2, 0);
}

void __fatal_error(const char *msg) {
    lcd_print_strn("\nFATAL ERROR:\n", 14);
    lcd_print_strn(msg, strlen(msg));
    for (;;) {
        flash_error(1);
    }
}

static mp_obj_t pyb_config_source_dir = MP_OBJ_NULL;
static mp_obj_t pyb_config_main = MP_OBJ_NULL;

mp_obj_t pyb_source_dir(mp_obj_t source_dir) {
    if (MP_OBJ_IS_STR(source_dir)) {
        pyb_config_source_dir = source_dir;
    }
    return mp_const_none;
}

mp_obj_t pyb_main(mp_obj_t main) {
    if (MP_OBJ_IS_STR(main)) {
        pyb_config_main = main;
    }
    return mp_const_none;
}

// sync all file systems
mp_obj_t pyb_sync(void) {
    storage_flush();
    return mp_const_none;
}

mp_obj_t pyb_delay(mp_obj_t count) {
    sys_tick_delay_ms(mp_obj_get_int(count));
    return mp_const_none;
}

void fatality(void) {
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

static const char fresh_main_py[] =
"# main.py -- put your code here!\n"
;

static const char *help_text =
"Welcome to Micro Python!\n\n"
"This is a *very* early version of Micro Python and has minimal functionality.\n\n"
"Specific commands for the board:\n"
"    pyb.info()     -- print some general information\n"
"    pyb.gc()       -- run the garbage collector\n"
"    pyb.delay(<n>) -- wait for n milliseconds\n"
"    pyb.Led(<n>)   -- create Led object for LED n (n=1,2)\n"
"                      Led methods: on(), off()\n"
"    pyb.Servo(<n>) -- create Servo object for servo n (n=1,2,3,4)\n"
"                      Servo methods: angle(<x>)\n"
"    pyb.switch()   -- return True/False if switch pressed or not\n"
"    pyb.accel()    -- get accelerometer values\n"
"    pyb.rand()     -- get a 16-bit random number\n"
"    pyb.gpio(<port>)           -- get port value (port='a4' for example)\n"
"    pyb.gpio(<port>, <val>)    -- set port value, True or False, 1 or 0\n"
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
        byte *id = (byte*)0x1fff7a10;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

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
        printf("_sidata=%p\n", &_sidata);
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
        printf("  %lu : %lu\n", info.used, info.free);
        printf("  1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
    }

    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("0:", &nclst, &fatfs);
        printf("LFS free: %u bytes\n", (uint)(nclst * fatfs->csize * 512));
    }

    return mp_const_none;
}

// SD card test
static mp_obj_t pyb_sd_test(void) {
    extern void sdio_init(void);
    sdio_init();
    return mp_const_none;
}

static void SYSCLKConfig_STOP(void) {
    /* After wake-up from STOP reconfigure the system clock */
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) {
    }

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08) {
    }
}

static mp_obj_t pyb_stop(void) {
    PWR_EnterSTANDBYMode();
    //PWR_FlashPowerDownCmd(ENABLE); don't know what the logic is with this

    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
     *        PLL as system clock source (HSE and PLL are disabled in STOP mode) */
    SYSCLKConfig_STOP();

    //PWR_FlashPowerDownCmd(DISABLE);

    return mp_const_none;
}

static mp_obj_t pyb_standby(void) {
    PWR_EnterSTANDBYMode();
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
    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_str(pyb_usart_global_debug, str);
    }
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
            } else if (pyb_usart_global_debug != PYB_USART_NONE && usart_rx_any(pyb_usart_global_debug)) {
                c = usart_rx_char(pyb_usart_global_debug);
                break;
            }
            sys_tick_delay_ms(1);
            if (storage_needs_flush()) {
                storage_flush();
            }
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
        sys_tick_delay_ms(10);
    }
}

void do_repl(void) {
    stdout_tx_str("Micro Python build <git hash> on 2/1/2014; PYBv3 with STM32F405RG\r\n");
    stdout_tx_str("Type \"help()\" for more information.\r\n");

    vstr_t line;
    vstr_init(&line, 32);

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
        qstr parse_exc_id;
        const char *parse_exc_msg;
        mp_parse_node_t pn = mp_parse(lex, MP_PARSE_SINGLE_INPUT, &parse_exc_id, &parse_exc_msg);
        qstr source_name = mp_lexer_source_name(lex);

        if (pn == MP_PARSE_NODE_NULL) {
            // parse error
            mp_lexer_show_error_pythonic_prefix(lex);
            printf("%s: %s\n", qstr_str(parse_exc_id), parse_exc_msg);
            mp_lexer_free(lex);
        } else {
            // parse okay
            mp_lexer_free(lex);
            mp_obj_t module_fun = mp_compile(pn, source_name, true);
            if (module_fun != mp_const_none) {
                nlr_buf_t nlr;
                uint32_t start = sys_tick_counter;
                if (nlr_push(&nlr) == 0) {
                    rt_call_function_0(module_fun);
                    nlr_pop();
                    // optional timing
                    if (0) {
                        uint32_t ticks = sys_tick_counter - start; // TODO implement a function that does this properly
                        printf("(took %lu ms)\n", ticks);
                    }
                } else {
                    // uncaught exception
                    mp_obj_print_exception((mp_obj_t)nlr.ret_val);
                }
            }
        }
    }

    stdout_tx_str("\r\n");
}

bool do_file(const char *filename) {
    mp_lexer_t *lex = mp_lexer_new_from_file(filename);

    if (lex == NULL) {
        printf("could not open file '%s' for reading\n", filename);
        return false;
    }

    qstr parse_exc_id;
    const char *parse_exc_msg;
    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT, &parse_exc_id, &parse_exc_msg);
    qstr source_name = mp_lexer_source_name(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error
        mp_lexer_show_error_pythonic_prefix(lex);
        printf("%s: %s\n", qstr_str(parse_exc_id), parse_exc_msg);
        mp_lexer_free(lex);
        return false;
    }

    mp_lexer_free(lex);

    mp_obj_t module_fun = mp_compile(pn, source_name, false);
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
        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
        return false;
    }
}

#define RAM_START (0x20000000) // fixed for chip
#define HEAP_END  (0x2001c000) // tunable
#define RAM_END   (0x20020000) // fixed for chip

void gc_helper_get_regs_and_clean_stack(machine_uint_t *regs, machine_uint_t heap_end);

void gc_collect(void) {
    uint32_t start = sys_tick_counter;
    gc_collect_start();
    gc_collect_root((void**)RAM_START, (((uint32_t)&_heap_start) - RAM_START) / 4);
    machine_uint_t regs[10];
    gc_helper_get_regs_and_clean_stack(regs, HEAP_END);
    gc_collect_root((void**)HEAP_END, (RAM_END - HEAP_END) / 4); // will trace regs since they now live in this function on the stack
    gc_collect_end();
    uint32_t ticks = sys_tick_counter - start; // TODO implement a function that does this properly

    if (0) {
        // print GC info
        gc_info_t info;
        gc_info(&info);
        printf("GC@%lu %lums\n", start, ticks);
        printf(" %lu total\n", info.total);
        printf(" %lu : %lu\n", info.used, info.free);
        printf(" 1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
    }
}

mp_obj_t pyb_gc(void) {
    gc_collect();
    return mp_const_none;
}

mp_obj_t pyb_gpio(uint n_args, mp_obj_t *args) {
    //assert(1 <= n_args && n_args <= 2);

    const char *pin_name = mp_obj_str_get_str(args[0]);
    GPIO_TypeDef *port;
    switch (pin_name[0]) {
        case 'A': case 'a': port = GPIOA; break;
        case 'B': case 'b': port = GPIOB; break;
        case 'C': case 'c': port = GPIOC; break;
        default: goto pin_error;
    }
    uint pin_num = 0;
    for (const char *s = pin_name + 1; *s; s++) {
        if (!('0' <= *s && *s <= '9')) {
            goto pin_error;
        }
        pin_num = 10 * pin_num + *s - '0';
    }
    if (!(0 <= pin_num && pin_num <= 15)) {
        goto pin_error;
    }

    if (n_args == 1) {
        // get pin
        if ((port->IDR & (1 << pin_num)) != (uint32_t)Bit_RESET) {
            return MP_OBJ_NEW_SMALL_INT(1);
        } else {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
    } else {
        // set pin
        if (rt_is_true(args[1])) {
            // set pin high
            port->BSRRL = 1 << pin_num;
        } else {
            // set pin low
            port->BSRRH = 1 << pin_num;
        }
        return mp_const_none;
    }

pin_error:
    nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_ValueError, "pin %s does not exist", pin_name));
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_obj, 1, 2, pyb_gpio);

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

static void rtc_init(void) {
    uint32_t rtc_clksrc;
    uint32_t timeout = 1000000;

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */
    while((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (--timeout > 0)) {
    }

    /* If LSE timed out, use LSI instead */
    if (timeout == 0) {
        /* Enable the LSI OSC */
        RCC_LSICmd(ENABLE);

        /* Wait till LSI is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
        }

        /* Use LSI as the RTC Clock Source */
        rtc_clksrc = RCC_RTCCLKSource_LSI;
    } else {
        /* Use LSE as the RTC Clock Source */
        rtc_clksrc = RCC_RTCCLKSource_LSE;
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(rtc_clksrc);

    /* Note: LSI is around (32KHz), these dividers should work either way */
    /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
    uint32_t uwSynchPrediv = 0xFF;
    uint32_t uwAsynchPrediv = 0x7F;

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    // Set the date (BCD)
    RTC_DateTypeDef RTC_DateStructure;
    RTC_DateStructure.RTC_Year = 0x13;
    RTC_DateStructure.RTC_Month = RTC_Month_October;
    RTC_DateStructure.RTC_Date = 0x26;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

    // Set the time (BCD)
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x01;
    RTC_TimeStructure.RTC_Minutes = 0x53;
    RTC_TimeStructure.RTC_Seconds = 0x00;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

    // Indicator for the RTC configuration
    //RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}

mp_obj_t pyb_rtc_read(void) {
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    printf("%02d:%02d:%02d\n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
    return mp_const_none;
}

typedef struct _pyb_file_obj_t {
    mp_obj_base_t base;
    FIL fp;
} pyb_file_obj_t;

void file_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    printf("<file %p>", self_in);
}

mp_obj_t file_obj_read(mp_obj_t self_in, mp_obj_t arg) {
    pyb_file_obj_t *self = self_in;
    int n = mp_obj_get_int(arg);
    byte *buf = m_new(byte, n);
    UINT n_out;
    f_read(&self->fp, buf, n, &n_out);
    return mp_obj_new_str(buf, n_out, false);
}

mp_obj_t file_obj_write(mp_obj_t self_in, mp_obj_t arg) {
    pyb_file_obj_t *self = self_in;
    uint l;
    const byte *s = mp_obj_str_get_data(arg, &l);
    UINT n_out;
    FRESULT res = f_write(&self->fp, s, l, &n_out);
    if (res != FR_OK) {
        printf("File error: could not write to file; error code %d\n", res);
    } else if (n_out != l) {
        printf("File error: could not write all data to file; wrote %d / %d bytes\n", n_out, l);
    }
    return mp_const_none;
}

mp_obj_t file_obj_close(mp_obj_t self_in) {
    pyb_file_obj_t *self = self_in;
    f_close(&self->fp);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_2(file_obj_read_obj, file_obj_read);
static MP_DEFINE_CONST_FUN_OBJ_2(file_obj_write_obj, file_obj_write);
static MP_DEFINE_CONST_FUN_OBJ_1(file_obj_close_obj, file_obj_close);

// TODO gc hook to close the file if not already closed

static const mp_method_t file_methods[] = {
    { "read", &file_obj_read_obj },
    { "write", &file_obj_write_obj },
    { "close", &file_obj_close_obj },
    {NULL, NULL},
};

static const mp_obj_type_t file_obj_type = {
    { &mp_const_type },
    "File",
    .print = file_obj_print,
    .methods = file_methods,
};

mp_obj_t pyb_io_open(mp_obj_t o_filename, mp_obj_t o_mode) {
    const char *filename = mp_obj_str_get_str(o_filename);
    const char *mode = mp_obj_str_get_str(o_mode);
    pyb_file_obj_t *self = m_new_obj(pyb_file_obj_t);
    self->base.type = &file_obj_type;
    if (mode[0] == 'r') {
        // open for reading
        FRESULT res = f_open(&self->fp, filename, FA_READ);
        if (res != FR_OK) {
            printf("FileNotFoundError: [Errno 2] No such file or directory: '%s'\n", filename);
            return mp_const_none;
        }
    } else if (mode[0] == 'w') {
        // open for writing, truncate the file first
        FRESULT res = f_open(&self->fp, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (res != FR_OK) {
            printf("?FileError: could not create file: '%s'\n", filename);
            return mp_const_none;
        }
    } else {
        printf("ValueError: invalid mode: '%s'\n", mode);
        return mp_const_none;
    }
    return self;
}

mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(RNG_GetRandomNumber() >> 16);
}

int main(void) {
    // TODO disable JTAG

    // update the SystemCoreClock variable
    SystemCoreClockUpdate();

    // set interrupt priority config to use all 4 bits for pre-empting
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // enable the CCM RAM and the GPIO's
    RCC->AHB1ENR |= RCC_AHB1ENR_CCMDATARAMEN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN
#if defined(STM32F4DISC)
        | RCC_AHB1ENR_GPIODEN
#endif
        ;

    // configure SDIO pins to be high to start with (apparently makes it more robust)
    {
      GPIO_InitTypeDef GPIO_InitStructure;
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(GPIOC, &GPIO_InitStructure);

      // Configure PD.02 CMD line
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
      GPIO_Init(GPIOD, &GPIO_InitStructure);
    }

    // basic sub-system init
    sys_tick_init();
    led_init();
    rtc_init();

    // turn on LED to indicate bootup
    led_state(PYB_LED_G1, 1);

    // more sub-system init
    switch_init();
    storage_init();

    // uncomment these 2 lines if you want REPL on USART_6 (or another usart) as well as on USB VCP
    //pyb_usart_global_debug = PYB_USART_3;
    //usart_init(pyb_usart_global_debug, 115200);

    int first_soft_reset = true;

soft_reset:

    // GC init
    gc_init(&_heap_start, (void*)HEAP_END);

    // Micro Python init
    qstr_init();
    rt_init();

    // LCD init (create in with LCD())
    lcd_init();

    // servo
    servo_init();

    // audio
    //audio_init();

    // timer
    timer_init();

    // RNG
    if (1) {
        RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
        RNG_Cmd(ENABLE);
    }

    // add some functions to the python namespace
    {
        rt_store_name(MP_QSTR_help, rt_make_function_n(0, pyb_help));

        mp_obj_t m = mp_obj_new_module(MP_QSTR_pyb);
        rt_store_attr(m, MP_QSTR_info, rt_make_function_n(0, pyb_info));
        rt_store_attr(m, MP_QSTR_sd_test, rt_make_function_n(0, pyb_sd_test));
        rt_store_attr(m, MP_QSTR_stop, rt_make_function_n(0, pyb_stop));
        rt_store_attr(m, MP_QSTR_standby, rt_make_function_n(0, pyb_standby));
        rt_store_attr(m, MP_QSTR_source_dir, rt_make_function_n(1, pyb_source_dir));
        rt_store_attr(m, MP_QSTR_main, rt_make_function_n(1, pyb_main));
        rt_store_attr(m, MP_QSTR_sync, rt_make_function_n(0, pyb_sync));
        rt_store_attr(m, MP_QSTR_gc, rt_make_function_n(0, pyb_gc));
        rt_store_attr(m, MP_QSTR_delay, rt_make_function_n(1, pyb_delay));
        rt_store_attr(m, MP_QSTR_switch, (mp_obj_t)&pyb_switch_obj);
        rt_store_attr(m, MP_QSTR_servo, rt_make_function_n(2, pyb_servo_set));
        rt_store_attr(m, MP_QSTR_pwm, rt_make_function_n(2, pyb_pwm_set));
#if MICROPY_HW_HAS_MMA7660
        rt_store_attr(m, MP_QSTR_accel, (mp_obj_t)&pyb_mma_read_obj);
        rt_store_attr(m, MP_QSTR_mma_read, (mp_obj_t)&pyb_mma_read_all_obj);
        rt_store_attr(m, MP_QSTR_mma_mode, (mp_obj_t)&pyb_mma_write_mode_obj);
#endif
        rt_store_attr(m, MP_QSTR_hid, rt_make_function_n(1, pyb_hid_send_report));
        rt_store_attr(m, MP_QSTR_time, rt_make_function_n(0, pyb_rtc_read));
        rt_store_attr(m, MP_QSTR_rand, rt_make_function_n(0, pyb_rng_get));
        rt_store_attr(m, MP_QSTR_Led, (mp_obj_t)&pyb_Led_obj);
        rt_store_attr(m, MP_QSTR_Servo, rt_make_function_n(1, pyb_Servo));
        rt_store_attr(m, MP_QSTR_I2C, rt_make_function_n(2, pyb_I2C));
        rt_store_attr(m, MP_QSTR_gpio, (mp_obj_t)&pyb_gpio_obj);
        rt_store_attr(m, MP_QSTR_Usart, rt_make_function_n(2, pyb_Usart));
        rt_store_attr(m, MP_QSTR_ADC, rt_make_function_n(1, pyb_ADC));
        rt_store_name(MP_QSTR_pyb, m);

        rt_store_name(MP_QSTR_open, rt_make_function_n(2, pyb_io_open));
    }

    // print a message to the LCD
    lcd_print_str(" micro py board\n");

    // check if user switch held (initiates reset of filesystem)
    bool reset_filesystem = false;
    if (switch_get()) {
        reset_filesystem = true;
        for (int i = 0; i < 50; i++) {
            if (!switch_get()) {
                reset_filesystem = false;
                break;
            }
            sys_tick_delay_ms(10);
        }
    }

    // local filesystem init
    {
        // try to mount the flash
        FRESULT res = f_mount(&fatfs0, "0:", 1);
        if (!reset_filesystem && res == FR_OK) {
            // mount sucessful
        } else if (reset_filesystem || res == FR_NO_FILESYSTEM) {
            // no filesystem, so create a fresh one
            // TODO doesn't seem to work correctly when reset_filesystem is true...

            // LED on to indicate creation of LFS
            led_state(PYB_LED_R2, 1);
            uint32_t stc = sys_tick_counter;

            res = f_mkfs("0:", 0, 0);
            if (res == FR_OK) {
                // success creating fresh LFS
            } else {
                __fatal_error("could not create LFS");
            }

            // create src directory
            res = f_mkdir("0:/src");
            // ignore result from mkdir

            // create empty main.py
            FIL fp;
            f_open(&fp, "0:/src/main.py", FA_WRITE | FA_CREATE_ALWAYS);
            UINT n;
            f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
            // TODO check we could write n bytes
            f_close(&fp);

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
            f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
            // TODO check we could write n bytes
            f_close(&fp);

            // keep LED on for at least 200ms
            sys_tick_wait_at_least(stc, 200);
            led_state(PYB_LED_R2, 0);
        }
    }

    // run /boot.py
    if (!do_file("0:/boot.py")) {
        flash_error(4);
    }

    // USB
    usb_init();

    // USB host; not working!
    //pyb_usbh_init();
    //rt_store_name(qstr_from_str("u_p"), rt_make_function_n(0, pyb_usbh_process));
    //rt_store_name(qstr_from_str("u_c"), rt_make_function_n(0, pyb_usbh_connect));

    if (first_soft_reset) {
#if MICROPY_HW_HAS_MMA7660
        // MMA: init and reset address to zero
        mma_init();
#endif
    }

    // turn boot-up LED off
    led_state(PYB_LED_G1, 0);

    // run main script
    {
        vstr_t *vstr = vstr_new();
        vstr_add_str(vstr, "0:/");
        if (pyb_config_source_dir == MP_OBJ_NULL) {
            vstr_add_str(vstr, "src");
        } else {
            vstr_add_str(vstr, mp_obj_str_get_str(pyb_config_source_dir));
        }
        vstr_add_char(vstr, '/');
        if (pyb_config_main == MP_OBJ_NULL) {
            vstr_add_str(vstr, "main.py");
        } else {
            vstr_add_str(vstr, mp_obj_str_get_str(pyb_config_main));
        }
        if (!do_file(vstr_str(vstr))) {
            flash_error(3);
        }
        vstr_free(vstr);
    }


#if MICROPY_HW_HAS_MMA7660
    // HID example
    if (0) {
        uint8_t data[4];
        data[0] = 0;
        data[1] = 1;
        data[2] = -2;
        data[3] = 0;
        for (;;) {
            if (switch_get()) {
                data[0] = 0x01; // 0x04 is middle, 0x02 is right
            } else {
                data[0] = 0x00;
            }
            mma_start(0x4c /* MMA_ADDR */, 1);
            mma_send_byte(0);
            mma_restart(0x4c /* MMA_ADDR */, 0);
            for (int i = 0; i <= 1; i++) {
                int v = mma_read_ack() & 0x3f;
                if (v & 0x20) {
                    v |= ~0x1f;
                }
                data[1 + i] = v;
            }
            mma_read_nack();
            usb_hid_send_report(data);
            sys_tick_delay_ms(15);
        }
    }
#endif

    // wifi
    //pyb_wlan_init();
    //pyb_wlan_start();

    do_repl();

    printf("PYB: sync filesystems\n");
    pyb_sync();

    printf("PYB: soft reboot\n");

    first_soft_reset = false;
    goto soft_reset;
}

// these 2 functions seem to actually work... no idea why
// replacing with libgcc does not work (probably due to wrong calling conventions)
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
