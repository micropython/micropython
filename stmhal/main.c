#include <stdio.h>
#include <string.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include "std.h"

#include "misc.h"
#include "systick.h"
#include "pendsv.h"
#include "mpconfig.h"
#include "qstr.h"
#include "nlr.h"
#include "misc.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "gc.h"
#include "gccollect.h"
#include "pyexec.h"
#include "pybmodule.h"
#include "usart.h"
#include "led.h"
#include "exti.h"
#include "usrsw.h"
#include "usb.h"
#include "rtc.h"
#include "sdcard.h"
#if 0
#include "ff.h"
#include "lexerfatfs.h"
#include "servo.h"
#include "lcd.h"
#include "storage.h"
#include "accel.h"
#include "timer.h"
#include "pybwlan.h"
#include "file.h"
#include "pin.h"
#endif

void SystemClock_Config(void);


int errno;

#if 0
static FATFS fatfs0;
#if MICROPY_HW_HAS_SDCARD
static FATFS fatfs1;
#endif
#endif

void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(PYB_LED_R1, 1);
        led_state(PYB_LED_R2, 0);
        HAL_Delay(250);
        led_state(PYB_LED_R1, 0);
        led_state(PYB_LED_R2, 1);
        HAL_Delay(250);
    }
    led_state(PYB_LED_R2, 0);
}

void __fatal_error(const char *msg) {
#if MICROPY_HW_HAS_LCD
#if 0
    lcd_print_strn("\nFATAL ERROR:\n", 14);
    lcd_print_strn(msg, strlen(msg));
#endif
#endif
    for (;;) {
        flash_error(1);
    }
}

STATIC mp_obj_t pyb_config_source_dir = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_main = MP_OBJ_NULL;

STATIC mp_obj_t pyb_source_dir(mp_obj_t source_dir) {
    if (MP_OBJ_IS_STR(source_dir)) {
        pyb_config_source_dir = source_dir;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_source_dir_obj, pyb_source_dir);

STATIC mp_obj_t pyb_main(mp_obj_t main) {
    if (MP_OBJ_IS_STR(main)) {
        pyb_config_main = main;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_main_obj, pyb_main);

void fatality(void) {
    led_state(PYB_LED_R1, 1);
    led_state(PYB_LED_G1, 1);
    led_state(PYB_LED_R2, 1);
    led_state(PYB_LED_G2, 1);
    for (;;) {
        flash_error(1);
    }
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
"    pyb.udelay(<n>) -- wait for n microseconds\n"
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

int main(void) {
    // TODO disable JTAG

    /* STM32F4xx HAL library initialization:
         - Configure the Flash prefetch, instruction and Data caches
         - Configure the Systick to generate an interrupt each 1 msec
         - Set NVIC Group Priority to 4
         - Global MSP (MCU Support Package) initialization
       */
    HAL_Init();

    // set the system clock to be HSE
    SystemClock_Config();

    // enable GPIO clocks
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    // enable the CCM RAM
    __CCMDATARAMEN_CLK_ENABLE();

    // some test code to flash LEDs
    led_init();

    led_state(0, 1);
    led_state(1, 0);
    led_state(2, 1);

#if 0
    for (;;) {
        HAL_Delay(500);
        led_state(1, 1);
        HAL_Delay(500);
        led_state(1, 0);
    }
#endif

#if 0
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
#endif

    // basic sub-system init
    pendsv_init();
    led_init();

    // turn on LED to indicate bootup
    led_state(PYB_LED_GREEN, 1);

#if MICROPY_HW_ENABLE_RTC
    rtc_init();
#endif

    // more sub-system init
#if MICROPY_HW_HAS_SDCARD
    sdcard_init();
#endif
#if 0
    storage_init();
#endif

    int first_soft_reset = true;

soft_reset:

    // GC init
    gc_init(&_heap_start, &_heap_end);

    // Change #if 0 to #if 1 if you want REPL on USART_6 (or another usart)
    // as well as on USB VCP
#if 0
    pyb_usart_global_debug = pyb_Usart(MP_OBJ_NEW_SMALL_INT(PYB_USART_YA),
                                       MP_OBJ_NEW_SMALL_INT(115200));
#else
    pyb_usart_global_debug = NULL;
#endif

    // Micro Python init
    qstr_init();
    rt_init();
    mp_obj_t def_path[3];
    def_path[0] = MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_);
    def_path[1] = MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_src);
    def_path[2] = MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_lib);
    sys_path = mp_obj_new_list(3, def_path);

    exti_init();

#if MICROPY_HW_HAS_SWITCH
    switch_init();
#endif

#if 0
#if MICROPY_HW_HAS_LCD
    // LCD init (just creates class, init hardware by calling LCD())
    lcd_init();
#endif

#if MICROPY_HW_ENABLE_SERVO
    // servo
    servo_init();
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

    pin_map_init();
#endif

    // add some functions to the builtin Python namespace
    rt_store_name(MP_QSTR_help, rt_make_function_n(0, pyb_help));
#if 0
    rt_store_name(MP_QSTR_open, rt_make_function_n(2, pyb_io_open));
#endif

    // we pre-import the pyb module
    // probably shouldn't do this, so we are compatible with CPython
    rt_store_name(MP_QSTR_pyb, (mp_obj_t)&pyb_module);

#if 0
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
            HAL_Delay(10);
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
            uint32_t start_tick = HAL_GetTick();

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
            sys_tick_wait_at_least(start_tick, 200);
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
            uint32_t start_tick = HAL_GetTick();

            FIL fp;
            f_open(&fp, "0:/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
            UINT n;
            f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
            // TODO check we could write n bytes
            f_close(&fp);

            // keep LED on for at least 200ms
            sys_tick_wait_at_least(start_tick, 200);
            led_state(PYB_LED_R2, 0);
        }
    }

    // run /boot.py
    if (!pyexec_file("0:/boot.py")) {
        flash_error(4);
    }

#endif
    if (first_soft_reset) {
#if 0
#if MICROPY_HW_HAS_MMA7660
        // MMA accel: init and reset address to zero
        accel_init();
#endif
#endif
    }

    // turn boot-up LED off
    led_state(PYB_LED_GREEN, 0);

#if 0
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
#endif

#if defined(USE_HOST_MODE)
    // USB host
    pyb_usb_host_init();
#elif defined(USE_DEVICE_MODE)
    // USB device
    pyb_usb_dev_init(PYB_USB_DEV_VCP_MSC);
#endif

#if 0
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
            HAL_Delay(15);
        }
    }
#endif

#if MICROPY_HW_HAS_WLAN
    // wifi
    pyb_wlan_init();
    pyb_wlan_start();
#endif
#endif

    pyexec_repl();

#if 0
    printf("PYB: sync filesystems\n");
    storage_flush();
#endif

    printf("PYB: soft reboot\n");

    first_soft_reset = false;
    goto soft_reset;
}
