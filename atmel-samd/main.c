#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"

#include "lib/fatfs/ff.h"
#include "lib/fatfs/diskio.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"
#include "extmod/fsusermount.h"

#include "asf/common/services/sleepmgr/sleepmgr.h"
#include "asf/common/services/usb/udc/udc.h"
#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "asf/sam0/drivers/system/system.h"
#include <board.h>

#include "common-hal/nativeio/AnalogIn.h"

#ifdef EXPRESS_BOARD
#include "common-hal/nativeio/types.h"
#include "QTouch/touch_api_ptc.h"
#endif

#include "autoreset.h"
#include "mpconfigboard.h"
#include "rgb_led_status.h"
#include "tick.h"

fs_user_mount_t fs_user_mount_flash;

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

extern void flash_init_vfs(fs_user_mount_t *vfs);

// we don't make this function static because it needs a lot of stack and we
// want it to be executed without using stack within main() function
void init_flash_fs(void) {
    // init the vfs object
    fs_user_mount_t *vfs = &fs_user_mount_flash;
    vfs->str = "/flash";
    vfs->len = 6;
    vfs->flags = 0;
    flash_init_vfs(vfs);

    // put the flash device in slot 0 (it will be unused at this point)
    MP_STATE_PORT(fs_user_mount)[0] = vfs;

    // try to mount the flash
    FRESULT res = f_mount(&vfs->fatfs, vfs->str, 1);

    if (res == FR_NO_FILESYSTEM) {
        // no filesystem, or asked to reset it, so create a fresh one

        // We are before USB initializes so temporarily undo the USB_WRITEABLE
        // requirement.
        bool usb_writeable = (vfs->flags & FSUSER_USB_WRITEABLE) > 0;
        vfs->flags &= ~FSUSER_USB_WRITEABLE;

        res = f_mkfs("/flash", 0, 0);
        if (res != FR_OK) {
            MP_STATE_PORT(fs_user_mount)[0] = NULL;
            return;
        }

        // set label
        f_setlabel("MICROPYTHON");

        if (usb_writeable) {
            vfs->flags |= FSUSER_USB_WRITEABLE;
        }
    } else if (res != FR_OK) {
        MP_STATE_PORT(fs_user_mount)[0] = NULL;
        return;
    }

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    f_chdrive("/flash");
}

static char *stack_top;
static char heap[16384];

void reset_mp(void) {
    new_status_color(0x8f008f);
    autoreset_stop();
    autoreset_enable();

    // Sync the file systems in case any used RAM from the GC to cache. As soon
    // as we re-init the GC all bets are off on the cache.
    disk_ioctl(0, CTRL_SYNC, NULL);
    disk_ioctl(1, CTRL_SYNC, NULL);
    disk_ioctl(2, CTRL_SYNC, NULL);

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));
    mp_obj_list_init(mp_sys_argv, 0);
}

#ifdef EXPRESS_BOARD
extern nativeio_touchin_obj_t *active_touchin_obj[DEF_SELFCAP_NUM_CHANNELS];
extern touch_selfcap_config_t selfcap_config;
#endif
void reset_samd21(void) {
    // Reset all SERCOMs except the one being used by the SPI flash.
    Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
        #ifdef SPI_FLASH_SERCOM
            if (sercom_instances[i] == SPI_FLASH_SERCOM) {
                continue;
            }
        #endif
        #ifdef MICROPY_HW_APA102_SERCOM
            if (sercom_instances[i] == MICROPY_HW_APA102_SERCOM) {
                continue;
            }
        #endif
        sercom_instances[i]->SPI.CTRLA.bit.SWRST = 1;
    }

#ifdef EXPRESS_BOARD
    touch_selfcap_sensors_deinit();
    for (int i = 0; i < selfcap_config.num_channels; i++) {
        active_touchin_obj[i] = NULL;
    }
    selfcap_config.num_channels = 0;
    selfcap_config.num_sensors = 0;
#endif

    analogin_reset();

    struct system_pinmux_config config;
    system_pinmux_get_config_defaults(&config);
    config.powersave = true;

    uint32_t pin_mask[2] = PORT_OUT_IMPLEMENTED;

    system_pinmux_group_set_config(&(PORT->Group[0]), pin_mask[0] & ~MICROPY_PORT_A, &config);
    system_pinmux_group_set_config(&(PORT->Group[1]), pin_mask[1] & ~MICROPY_PORT_B, &config);
}

bool maybe_run(const char* filename, pyexec_result_t* exec_result) {
    FILINFO fno;
#if _USE_LFN
    fno.lfname = NULL;
    fno.lfsize = 0;
#endif
    FRESULT res = f_stat(filename, &fno);
    if (res != FR_OK || fno.fattrib & AM_DIR) {
        return false;
    }
    mp_hal_stdout_tx_str(filename);
    mp_hal_stdout_tx_str(" output:\r\n");
    pyexec_file(filename, exec_result);
    return true;
}

bool start_mp(void) {
    bool cdc_enabled_at_start = mp_cdc_enabled;
    #ifdef AUTORESET_DELAY_MS
    if (cdc_enabled_at_start) {
        mp_hal_stdout_tx_str("\r\n");
        mp_hal_stdout_tx_str("Auto-soft reset is on. Simply save files over USB to run them.\r\n");
    }
    #endif

    new_status_color(BOOT_RUNNING);
    pyexec_result_t result;
    bool found_boot = maybe_run("settings.txt", &result) ||
                      maybe_run("settings.py", &result) ||
                      maybe_run("boot.py", &result) ||
                      maybe_run("boot.txt", &result);
    bool found_main = false;
    if (!found_boot || !(result.return_code & PYEXEC_FORCED_EXIT)) {
        new_status_color(MAIN_RUNNING);
        found_main = maybe_run("code.txt", &result) ||
                     maybe_run("code.py", &result) ||
                     maybe_run("main.py", &result) ||
                     maybe_run("main.txt", &result);
    }

    if (result.return_code & PYEXEC_FORCED_EXIT) {
        return reset_next_character;
    }

    // If not is USB mode then do not skip the repl.
    #ifndef USB_REPL
    return false;
    #endif

    // Wait for connection or character.
    bool cdc_enabled_before = false;
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    new_status_color(ALL_DONE);
    uint32_t pattern_start = ticks_ms;

    uint32_t total_exception_cycle = 0;
    uint8_t ones = result.exception_line % 10;
    ones += ones > 0 ? 1 : 0;
    uint8_t tens = (result.exception_line / 10) % 10;
    tens += tens > 0 ? 1 : 0;
    uint8_t hundreds = (result.exception_line / 100) % 10;
    hundreds += hundreds > 0 ? 1 : 0;
    uint8_t thousands = (result.exception_line / 1000) % 10;
    thousands += thousands > 0 ? 1 : 0;
    uint8_t digit_sum = ones + tens + hundreds + thousands;
    uint8_t num_places = 0;
    uint16_t line = result.exception_line;
    for (int i = 0; i < 4; i++) {
        if ((line % 10) > 0) {
            num_places++;
        }
        line /= 10;
    }
    if (result.return_code == PYEXEC_EXCEPTION) {
        total_exception_cycle = EXCEPTION_TYPE_LENGTH_MS * 3 + LINE_NUMBER_TOGGLE_LENGTH * digit_sum + LINE_NUMBER_TOGGLE_LENGTH * num_places;
    }
    #endif
    while (true) {
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
        if (reset_next_character) {
            return true;
        }
        if (usb_rx_count > 0) {
            // Skip REPL if reset was requested.
            return receive_usb() == CHAR_CTRL_D;
        }

        if (!cdc_enabled_before && mp_cdc_enabled) {
            if (cdc_enabled_at_start) {
                mp_hal_stdout_tx_str("\r\n\r\n");
            } else {
                mp_hal_stdout_tx_str("Auto-soft reset is on. Simply save files over USB to run them.\r\n");
            }
            mp_hal_stdout_tx_str("Press any key to enter the REPL and disable auto-reset. Use CTRL-D to soft reset.\r\n");
        }
        if (cdc_enabled_before && !mp_cdc_enabled) {
            cdc_enabled_at_start = false;
        }
        cdc_enabled_before = mp_cdc_enabled;

        #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
        uint32_t tick_diff = ticks_ms - pattern_start;
        if (result.return_code != PYEXEC_EXCEPTION) {
            // All is good. Ramp ALL_DONE up and down.
            if (tick_diff > ALL_GOOD_CYCLE_MS) {
                pattern_start = ticks_ms;
                tick_diff = 0;
            }

            uint16_t brightness = tick_diff * 255 / (ALL_GOOD_CYCLE_MS / 2);
            if (brightness > 255) {
                brightness = 511 - brightness;
            }
            new_status_color(color_brightness(ALL_DONE, brightness));
        } else {
            if (tick_diff > total_exception_cycle) {
                pattern_start = ticks_ms;
                tick_diff = 0;
            }
            // First flash the file color.
            if (tick_diff < EXCEPTION_TYPE_LENGTH_MS) {
                if (found_main) {
                    new_status_color(MAIN_RUNNING);
                } else {
                    new_status_color(BOOT_RUNNING);
                }
            // Next flash the exception color.
            } else if (tick_diff < EXCEPTION_TYPE_LENGTH_MS * 2) {
                if (mp_obj_is_subclass_fast(result.exception_type, &mp_type_IndentationError)) {
                    new_status_color(INDENTATION_ERROR);
                } else if (mp_obj_is_subclass_fast(result.exception_type, &mp_type_SyntaxError)) {
                    new_status_color(SYNTAX_ERROR);
                } else if (mp_obj_is_subclass_fast(result.exception_type, &mp_type_NameError)) {
                    new_status_color(NAME_ERROR);
                } else if (mp_obj_is_subclass_fast(result.exception_type, &mp_type_OSError)) {
                    new_status_color(OS_ERROR);
                } else if (mp_obj_is_subclass_fast(result.exception_type, &mp_type_ValueError)) {
                    new_status_color(VALUE_ERROR);
                } else {
                    new_status_color(OTHER_ERROR);
                }
            // Finally flash the line number digits from highest to lowest.
            // Zeroes will not produce a flash but can be read by the absence of
            // a color from the sequence.
            } else if (tick_diff < (EXCEPTION_TYPE_LENGTH_MS * 2 + LINE_NUMBER_TOGGLE_LENGTH * digit_sum)) {
                uint32_t digit_diff = tick_diff - EXCEPTION_TYPE_LENGTH_MS * 2;
                if ((digit_diff % LINE_NUMBER_TOGGLE_LENGTH) < (LINE_NUMBER_TOGGLE_LENGTH / 2)) {
                    new_status_color(BLACK);
                } else if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * thousands) {
                    if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH) {
                        new_status_color(BLACK);
                    } else {
                        new_status_color(THOUSANDS);
                    }
                } else if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (thousands + hundreds)) {
                    if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (thousands + 1)) {
                        new_status_color(BLACK);
                    } else {
                        new_status_color(HUNDREDS);
                    }
                } else if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (thousands + hundreds + tens)) {
                    if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (thousands + hundreds + 1)) {
                        new_status_color(BLACK);
                    } else {
                        new_status_color(TENS);
                    }
                } else {
                    if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (thousands + hundreds + tens + 1)) {
                        new_status_color(BLACK);
                    } else {
                        new_status_color(ONES);
                    }
                }
            } else {
                new_status_color(BLACK);
            }
        }
        #else
        (void) found_main; // Pretend to use found_main so the compiler doesn't complain.
        #endif
    }
}

#ifdef UART_REPL
struct usart_module usart_instance;
#endif

#ifdef ENABLE_MICRO_TRACE_BUFFER
// Stores 2 ^ TRACE_BUFFER_MAGNITUDE_PACKETS packets.
// 7 -> 128 packets
#define TRACE_BUFFER_MAGNITUDE_PACKETS 7
// Size in uint32_t. Two per packet.
#define TRACE_BUFFER_SIZE (1 << (TRACE_BUFFER_MAGNITUDE_PACKETS + 1))
// Size in bytes. 4 bytes per uint32_t.
#define TRACE_BUFFER_SIZE_BYTES (TRACE_BUFFER_SIZE << 2)
__attribute__((__aligned__(TRACE_BUFFER_SIZE_BYTES))) uint32_t mtb[TRACE_BUFFER_SIZE];
#endif

// Serial number as hex characters.
char serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH];
void load_serial_number(void) {
    char nibble_to_hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
        'B', 'C', 'D', 'E', 'F'};
    uint32_t* addresses[4] = {(uint32_t *) 0x0080A00C, (uint32_t *) 0x0080A040,
                              (uint32_t *) 0x0080A044, (uint32_t *) 0x0080A048};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            uint8_t nibble = (*(addresses[i]) >> j * 4) & 0xf;
            serial_number[i * 8 + j] = nibble_to_hex[nibble];
        }
    }
}

void samd21_init(void) {
#ifdef ENABLE_MICRO_TRACE_BUFFER
    REG_MTB_POSITION = ((uint32_t) (mtb - REG_MTB_BASE)) & 0xFFFFFFF8;
    REG_MTB_FLOW = (((uint32_t) mtb - REG_MTB_BASE) + TRACE_BUFFER_SIZE_BYTES) & 0xFFFFFFF8;
    REG_MTB_MASTER = 0x80000000 + (TRACE_BUFFER_MAGNITUDE_PACKETS - 1);
#endif

    load_serial_number();

    irq_initialize_vectors();
    cpu_irq_enable();

    // Initialize the sleep manager
    sleepmgr_init();

    system_init();

    delay_init();

    board_init();

    // Configure millisecond timer initialization.
    tick_init();

    // Uncomment to init PIN_PA17 for debugging.
    // struct port_config pin_conf;
    // port_get_config_defaults(&pin_conf);
    //
    // pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    // port_pin_set_config(MICROPY_HW_LED1, &pin_conf);
    // port_pin_set_output_level(MICROPY_HW_LED1, false);

    rgb_led_status_init();
}

int main(int argc, char **argv) {
    // initialise the cpu and peripherals
    samd21_init();

    int stack_dummy;
    // Store the location of stack_dummy as an approximation for the top of the
    // stack so the GC can account for objects that may be referenced by the
    // stack between here and where gc_collect is called.
    stack_top = (char*)&stack_dummy;

    // Initialise the local flash filesystem after the gc in case we need to
    // grab memory from it. Create it if needed, mount in on /flash, and set it
    // as current dir.
    init_flash_fs();

    // Start USB after getting everything going.
    #ifdef USB_REPL
        udc_start();
    #endif

    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
    int exit_code = PYEXEC_FORCED_EXIT;
    bool skip_repl = true;
    bool first_run = true;
    for (;;) {
        if (!skip_repl) {
            autoreset_disable();
            new_status_color(REPL_RUNNING);
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                exit_code = pyexec_raw_repl();
            } else {
                exit_code = pyexec_friendly_repl();
            }
        }
        if (exit_code == PYEXEC_FORCED_EXIT) {
            if (!first_run) {
                mp_hal_stdout_tx_str("soft reboot\r\n");
            }
            reset_samd21();
            reset_mp();
            first_run = false;
            skip_repl = start_mp();
        } else if (exit_code != 0) {
            break;
        }
    }
    mp_deinit();
    return 0;
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    // This naively collects all object references from an approximate stack
    // range.
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

mp_import_stat_t fat_vfs_import_stat(const char *path);
mp_import_stat_t mp_import_stat(const char *path) {
    #if MICROPY_VFS_FAT
    return fat_vfs_import_stat(path);
    #else
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
    #endif
}

void nlr_jump_fail(void *val) {
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
