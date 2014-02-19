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
#include <usbd_storage_msd.h>
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
#include "parsehelper.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "gc.h"
#include "gccollect.h"
#include "systick.h"
#include "pendsv.h"
#include "pyexec.h"
#include "led.h"
#include "gpio.h"
#include "servo.h"
#include "lcd.h"
#include "storage.h"
#include "sdcard.h"
#include "accel.h"
#include "usart.h"
#include "usb.h"
#include "timer.h"
#include "audio.h"
#include "pybwlan.h"
#include "i2c.h"
#include "usrsw.h"
#include "adc.h"
#include "rtc.h"
#include "file.h"
#include "pin.h"

int errno;

static FATFS fatfs0;
#if MICROPY_HW_HAS_SDCARD
static FATFS fatfs1;
#endif

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
#if MICROPY_HW_HAS_LCD
    lcd_print_strn("\nFATAL ERROR:\n", 14);
    lcd_print_strn(msg, strlen(msg));
#endif
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
"    pyb.repl_info(<val>) -- enable/disable printing of info after each command\n"
"    pyb.delay(<n>) -- wait for n milliseconds\n"
"    pyb.Led(<n>)   -- create Led object for LED n (n=1,2)\n"
"                      Led methods: on(), off()\n"
"    pyb.Servo(<n>) -- create Servo object for servo n (n=1,2,3,4)\n"
"                      Servo methods: angle(<x>)\n"
"    pyb.switch()   -- return True/False if switch pressed or not\n"
"    pyb.accel()    -- get accelerometer values\n"
"    pyb.rand()     -- get a 16-bit random number\n"
"    pyb.gpio(<port>)           -- get port value (port='A4' for example)\n"
"    pyb.gpio(<port>, <val>)    -- set port value, True or False, 1 or 0\n"
"    pyb.ADC(<port>) -- make an analog port object (port='C0' for example)\n"
"                       ADC methods: read()\n"
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
        printf("_text_end=%p\n", &_text_end);
        printf("_data_start_init=%p\n", &_data_start_init);
        printf("_data_start=%p\n", &_data_start);
        printf("_data_end=%p\n", &_data_end);
        printf("_bss_start=%p\n", &_bss_start);
        printf("_bss_end=%p\n", &_bss_end);
        printf("_stack_end=%p\n", &_stack_end);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        uint n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
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

mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(RNG_GetRandomNumber() >> 16);
}

mp_obj_t pyb_millis(void) {
    return mp_obj_new_int(sys_tick_counter);
}

int main(void) {
    // TODO disable JTAG

    // update the SystemCoreClock variable
    SystemCoreClockUpdate();

    // set interrupt priority config to use all 4 bits for pre-empting
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // enable the CCM RAM and the GPIO's
    RCC->AHB1ENR |= RCC_AHB1ENR_CCMDATARAMEN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;

#if MICROPY_HW_HAS_SDCARD
    {
        // configure SDIO pins to be high to start with (apparently makes it more robust)
        // FIXME this is not making them high, it just makes them outputs...
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
#endif
#if defined(NETDUINO_PLUS_2)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

#if MICROPY_HW_HAS_SDCARD
        // Turn on the power enable for the sdcard (PB1)
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
#endif

        // Turn on the power for the 5V on the expansion header (PB2)
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_SET);
    }
#endif

    // basic sub-system init
    sys_tick_init();
    pendsv_init();
    led_init();

#if MICROPY_HW_ENABLE_RTC
    rtc_init();
#endif

    // turn on LED to indicate bootup
    led_state(PYB_LED_G1, 1);

    // more sub-system init
#if MICROPY_HW_HAS_SWITCH
    switch_init();
#endif
#if MICROPY_HW_HAS_SDCARD
    sdcard_init();
#endif
    storage_init();

    // uncomment these 2 lines if you want REPL on USART_6 (or another usart) as well as on USB VCP
    //pyb_usart_global_debug = PYB_USART_3;
    //usart_init(pyb_usart_global_debug, 115200);

    int first_soft_reset = true;

soft_reset:

    // GC init
    gc_init(&_heap_start, &_heap_end);

    // Micro Python init
    qstr_init();
    rt_init();
    mp_obj_t def_path[3];
    def_path[0] = MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_);
    def_path[1] = MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_src);
    def_path[2] = MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_lib);
    sys_path = mp_obj_new_list(3, def_path);

#if MICROPY_HW_HAS_LCD
    // LCD init (just creates class, init hardware by calling LCD())
    lcd_init();
#endif

#if MICROPY_HW_ENABLE_SERVO
    // servo
    servo_init();
#endif

#if MICROPY_HW_ENABLE_AUDIO
    // audio
    audio_init();
#endif

#if MICROPY_HW_ENABLE_TIMER
    // timer
    timer_init();
#endif

#if MICROPY_HW_ENABLE_RNG
    // RNG
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
#endif

    // add some functions to the python namespace
    {
        rt_store_name(MP_QSTR_help, rt_make_function_n(0, pyb_help));

        mp_obj_t m = mp_obj_new_module(MP_QSTR_pyb);
        rt_store_attr(m, MP_QSTR_info, rt_make_function_n(0, pyb_info));
        rt_store_attr(m, MP_QSTR_gc, (mp_obj_t)&pyb_gc_obj);
        rt_store_attr(m, qstr_from_str("repl_info"), rt_make_function_n(1, pyb_set_repl_info));
#if MICROPY_HW_HAS_SDCARD
        rt_store_attr(m, qstr_from_str("SD"), (mp_obj_t)&pyb_sdcard_obj);
#endif
        rt_store_attr(m, MP_QSTR_stop, rt_make_function_n(0, pyb_stop));
        rt_store_attr(m, MP_QSTR_standby, rt_make_function_n(0, pyb_standby));
        rt_store_attr(m, MP_QSTR_source_dir, rt_make_function_n(1, pyb_source_dir));
        rt_store_attr(m, MP_QSTR_main, rt_make_function_n(1, pyb_main));
        rt_store_attr(m, MP_QSTR_sync, rt_make_function_n(0, pyb_sync));
        rt_store_attr(m, MP_QSTR_delay, rt_make_function_n(1, pyb_delay));
#if MICROPY_HW_HAS_SWITCH
        rt_store_attr(m, MP_QSTR_switch, (mp_obj_t)&pyb_switch_obj);
#endif
#if MICROPY_HW_ENABLE_SERVO
        rt_store_attr(m, MP_QSTR_servo, rt_make_function_n(2, pyb_servo_set));
#endif
        rt_store_attr(m, MP_QSTR_pwm, rt_make_function_n(2, pyb_pwm_set));
#if MICROPY_HW_HAS_MMA7660
        rt_store_attr(m, MP_QSTR_accel, (mp_obj_t)&pyb_accel_read_obj);
        rt_store_attr(m, MP_QSTR_accel_read, (mp_obj_t)&pyb_accel_read_all_obj);
        rt_store_attr(m, MP_QSTR_accel_mode, (mp_obj_t)&pyb_accel_write_mode_obj);
#endif
        rt_store_attr(m, MP_QSTR_hid, rt_make_function_n(1, pyb_hid_send_report));
#if MICROPY_HW_ENABLE_RTC
        rt_store_attr(m, MP_QSTR_time, (mp_obj_t)&pyb_rtc_read_obj);
        rt_store_attr(m, qstr_from_str("rtc_info"), (mp_obj_t)&pyb_rtc_info_obj);
#endif
#if MICROPY_HW_ENABLE_RNG
        rt_store_attr(m, MP_QSTR_rand, rt_make_function_n(0, pyb_rng_get));
#endif
        rt_store_attr(m, MP_QSTR_Led, (mp_obj_t)&pyb_Led_obj);
#if MICROPY_HW_ENABLE_SERVO
        rt_store_attr(m, MP_QSTR_Servo, rt_make_function_n(1, pyb_Servo));
#endif
        rt_store_attr(m, MP_QSTR_I2C, rt_make_function_n(2, pyb_I2C));
        rt_store_attr(m, MP_QSTR_Usart, rt_make_function_n(2, pyb_Usart));
        rt_store_attr(m, qstr_from_str("ADC_all"), (mp_obj_t)&pyb_ADC_all_obj);
        rt_store_attr(m, MP_QSTR_ADC, (mp_obj_t)&pyb_ADC_obj);
        rt_store_attr(m, qstr_from_str("millis"), rt_make_function_n(0, pyb_millis));

        pin_map_init(m);
        gpio_init(m);

        rt_store_name(MP_QSTR_pyb, m);

        rt_store_name(MP_QSTR_open, rt_make_function_n(2, pyb_io_open));
    }

    // check if user switch held (initiates reset of filesystem)
    bool reset_filesystem = false;
#if MICROPY_HW_HAS_SWITCH
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
#endif
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
    if (!pyexec_file("0:/boot.py")) {
        flash_error(4);
    }

    if (first_soft_reset) {
#if MICROPY_HW_HAS_MMA7660
        // MMA accel: init and reset address to zero
        accel_init();
#endif
    }

    // turn boot-up LED off
    led_state(PYB_LED_G1, 0);

#if MICROPY_HW_HAS_SDCARD
    // if an SD card is present then mount it on 1:/
    if (sdcard_is_present()) {
        FRESULT res = f_mount(&fatfs1, "1:", 1);
        if (res != FR_OK) {
            printf("[SD] could not mount SD card\n");
        } else {
            if (first_soft_reset) {
                // use SD card as medium for the USB MSD
                usbd_storage_select_medium(USBD_STORAGE_MEDIUM_SDCARD);
            }
        }
    }
#endif

#ifdef USE_HOST_MODE
    // USB host
    pyb_usb_host_init();
#elif defined(USE_DEVICE_MODE)
    // USB device
    pyb_usb_dev_init();
#endif

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
        if (!pyexec_file(vstr_str(vstr))) {
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
        #if MICROPY_HW_HAS_SWITCH
            if (switch_get()) {
                data[0] = 0x01; // 0x04 is middle, 0x02 is right
            } else {
                data[0] = 0x00;
            }
        #else
            data[0] = 0x00;
        #endif
            accel_start(0x4c /* ACCEL_ADDR */, 1);
            accel_send_byte(0);
            accel_restart(0x4c /* ACCEL_ADDR */, 0);
            for (int i = 0; i <= 1; i++) {
                int v = accel_read_ack() & 0x3f;
                if (v & 0x20) {
                    v |= ~0x1f;
                }
                data[1 + i] = v;
            }
            accel_read_nack();
            usb_hid_send_report(data);
            sys_tick_delay_ms(15);
        }
    }
#endif

#if MICROPY_HW_HAS_WLAN
    // wifi
    pyb_wlan_init();
    pyb_wlan_start();
#endif

    pyexec_repl();

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
