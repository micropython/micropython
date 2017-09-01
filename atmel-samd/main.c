/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/frozenmod.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"

#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"

#include "asf/common/services/sleepmgr/sleepmgr.h"
#include "asf/common/services/usb/udc/udc.h"
#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/nvm/nvm.h"
#include "asf/sam0/drivers/port/port.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "asf/sam0/drivers/system/system.h"
#include <board.h>

#include "boards/board.h"

#include "common-hal/analogio/AnalogIn.h"
#include "common-hal/audioio/AudioOut.h"
#include "common-hal/audiobusio/PDMIn.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PWMOut.h"
#include "common-hal/usb_hid/__init__.h"

#include "autoreload.h"
#include "flash_api.h"
#include "mpconfigboard.h"
#include "rgb_led_status.h"
#include "shared_dma.h"
#include "tick.h"

#ifdef EXPRESS_BOARD
#include "common-hal/touchio/TouchIn.h"
#define INTERNAL_CIRCUITPY_CONFIG_START_ADDR (0x00040000 - 0x100 - CIRCUITPY_INTERNAL_NVM_SIZE)
#else
#define INTERNAL_CIRCUITPY_CONFIG_START_ADDR (0x00040000 - 0x010000 - 0x100 - CIRCUITPY_INTERNAL_NVM_SIZE)
#endif

fs_user_mount_t fs_user_mount_flash;
mp_vfs_mount_t mp_vfs_mount_flash;

typedef enum {
    NO_SAFE_MODE = 0,
    BROWNOUT,
    HARD_CRASH,
    USER_SAFE_MODE,
} safe_mode_t;

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

// we don't make this function static because it needs a lot of stack and we
// want it to be executed without using stack within main() function
void init_flash_fs(void) {
    // init the vfs object
    fs_user_mount_t *vfs_fat = &fs_user_mount_flash;
    vfs_fat->flags = 0;
    flash_init_vfs(vfs_fat);

    // try to mount the flash
    FRESULT res = f_mount(&vfs_fat->fatfs);

    if (res == FR_NO_FILESYSTEM) {
        // no filesystem so create a fresh one

        uint8_t working_buf[_MAX_SS];
        res = f_mkfs(&vfs_fat->fatfs, FM_FAT, 0, working_buf, sizeof(working_buf));
        // Flush the new file system to make sure its repaired immediately.
        flash_flush();
        if (res != FR_OK) {
            return;
        }

        // set label
        f_setlabel(&vfs_fat->fatfs, "CIRCUITPY");
    } else if (res != FR_OK) {
        return;
    }
    mp_vfs_mount_t *vfs = &mp_vfs_mount_flash;
    vfs->str = "/";
    vfs->len = 1;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
    vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = vfs;
}

static char heap[16384 + 4096];

void reset_mp(void) {
    reset_status_led();
    new_status_color(0x8f008f);
    autoreload_stop();

    // Sync the file systems in case any used RAM from the GC to cache. As soon
    // as we re-init the GC all bets are off on the cache.
    flash_flush();

    // Clear the readline history. It references the heap we're about to destroy.
    readline_init0();

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    // Frozen modules are in their own pseudo-dir, e.g., ".frozen".
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_FROZEN_FAKE_DIR_QSTR));

    mp_obj_list_init(mp_sys_argv, 0);
}

extern volatile bool mp_msc_enabled;

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
    audioout_reset();
    touchin_reset();
    pdmin_reset();
    pulsein_reset();
    pulseout_reset();
    pwmout_reset();
#endif

    analogin_reset();


    // Wait for the DAC to sync then reset.
    while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {}
    DAC->CTRLA.reg |= DAC_CTRLA_SWRST;

    reset_all_pins();


    usb_hid_reset();

#ifdef CALIBRATE_CRYSTALLESS
    // If we are on USB lets double check our fine calibration for the clock and
    // save the new value if its different enough.
    if (mp_msc_enabled) {
        SYSCTRL->DFLLSYNC.bit.READREQ = 1;
        uint16_t saved_calibration = 0x1ff;
        if (strcmp((char*) INTERNAL_CIRCUITPY_CONFIG_START_ADDR, "CIRCUITPYTHON1") == 0) {
            saved_calibration = ((uint16_t *) INTERNAL_CIRCUITPY_CONFIG_START_ADDR)[8];
        }
        while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0) {
            // TODO(tannewt): Run the mass storage stuff if this takes a while.
        }
        int16_t current_calibration = SYSCTRL->DFLLVAL.bit.FINE;
        if (abs(current_calibration - saved_calibration) > 10) {
            enum status_code error_code;
            uint8_t page_buffer[NVMCTRL_ROW_SIZE];
            for (int i = 0; i < NVMCTRL_ROW_PAGES; i++) {
                do
                {
                    error_code = nvm_read_buffer(INTERNAL_CIRCUITPY_CONFIG_START_ADDR + i * NVMCTRL_PAGE_SIZE,
                                                 page_buffer + i * NVMCTRL_PAGE_SIZE,
                                                 NVMCTRL_PAGE_SIZE);
                } while (error_code == STATUS_BUSY);
            }
            // If this is the first write, include the header.
            if (strcmp((char*) page_buffer, "CIRCUITPYTHON1") != 0) {
                memcpy(page_buffer, "CIRCUITPYTHON1", 15);
            }
            // First 16 bytes (0-15) are ID. Little endian!
            page_buffer[16] = current_calibration & 0xff;
            page_buffer[17] = current_calibration >> 8;
            do
            {
                error_code = nvm_erase_row(INTERNAL_CIRCUITPY_CONFIG_START_ADDR);
            } while (error_code == STATUS_BUSY);
            for (int i = 0; i < NVMCTRL_ROW_PAGES; i++) {
                do
                {
                    error_code = nvm_write_buffer(INTERNAL_CIRCUITPY_CONFIG_START_ADDR + i * NVMCTRL_PAGE_SIZE,
                                                  page_buffer + i * NVMCTRL_PAGE_SIZE,
                                                  NVMCTRL_PAGE_SIZE);
                } while (error_code == STATUS_BUSY);
            }
        }
    }
#endif
}

bool maybe_run(const char* filename, pyexec_result_t* exec_result) {
    mp_import_stat_t stat = mp_import_stat(filename);
    if (stat != MP_IMPORT_STAT_FILE) {
        return false;
    }
    mp_hal_stdout_tx_str(filename);
    mp_hal_stdout_tx_str(" output:\r\n");
    pyexec_file(filename, exec_result);
    return true;
}

bool start_mp(safe_mode_t safe_mode) {
    bool cdc_enabled_at_start = mp_cdc_enabled;
    #ifdef CIRCUITPY_AUTORELOAD_DELAY_MS
    if (cdc_enabled_at_start) {
        mp_hal_stdout_tx_str("\r\n");
        if (autoreload_is_enabled()) {
            mp_hal_stdout_tx_str("Auto-reload is on. Simply save files over USB to run them or enter REPL to disable.\r\n");
        } else if (safe_mode != NO_SAFE_MODE) {
            mp_hal_stdout_tx_str("Running in safe mode! Auto-reload is off.\r\n");
        } else if (!autoreload_is_enabled()) {
            mp_hal_stdout_tx_str("Auto-reload is off.\r\n");
        }
    }
    #endif

    pyexec_result_t result;
    bool found_main = false;
    if (safe_mode != NO_SAFE_MODE) {
        mp_hal_stdout_tx_str("Running in safe mode! Not running saved code.\r\n");
    } else {
        new_status_color(MAIN_RUNNING);
        found_main = maybe_run("code.txt", &result) ||
                     maybe_run("code.py", &result) ||
                     maybe_run("main.py", &result) ||
                     maybe_run("main.txt", &result);
        reset_status_led();

        if (result.return_code & PYEXEC_FORCED_EXIT) {
            return reload_next_character;
        }

        // If not is USB mode then do not skip the repl.
        #ifndef USB_REPL
        return false;
        #endif
    }

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
        if (reload_next_character) {
            return true;
        }
        if (usb_rx_count > 0) {
            // Skip REPL if reload was requested.
            return receive_usb() == CHAR_CTRL_D;
        }

        if (!cdc_enabled_before && mp_cdc_enabled) {
            if (cdc_enabled_at_start) {
                mp_hal_stdout_tx_str("\r\n\r\n");
            }

            if (!cdc_enabled_at_start) {
                if (autoreload_is_enabled()) {
                    mp_hal_stdout_tx_str("Auto-reload is on. Simply save files over USB to run them or enter REPL to disable.\r\n");
                } else {
                    mp_hal_stdout_tx_str("Auto-reload is off.\r\n");
                }
            }
            // Output a user safe mode string if its set.
            #ifdef BOARD_USER_SAFE_MODE
            if (safe_mode == USER_SAFE_MODE) {
                mp_hal_stdout_tx_str("\r\nYou requested starting safe mode by ");
                mp_hal_stdout_tx_str(BOARD_USER_SAFE_MODE);
                mp_hal_stdout_tx_str(".\r\nTo exit, please reset the board without ");
                mp_hal_stdout_tx_str(BOARD_USER_SAFE_MODE);
                mp_hal_stdout_tx_str(".\r\n");
            } else
            #endif
            if (safe_mode != NO_SAFE_MODE) {
                mp_hal_stdout_tx_str("\r\nYou are running in safe mode which means something really bad happened.\r\n");
                if (safe_mode == HARD_CRASH) {
                    mp_hal_stdout_tx_str("Looks like our core CircuitPython code crashed hard. Whoops!\r\n");
                    mp_hal_stdout_tx_str("Please file an issue here with the contents of your CIRCUITPY drive:\r\n");
                    mp_hal_stdout_tx_str("https://github.com/adafruit/circuitpython/issues\r\n");
                } else if (safe_mode == BROWNOUT) {
                    mp_hal_stdout_tx_str("The microcontroller's power dipped. Please make sure your power supply provides \r\n");
                    mp_hal_stdout_tx_str("enough power for the whole circuit and press reset (after ejecting CIRCUITPY).\r\n");
                }
            }
            mp_hal_stdout_tx_str("\r\nPress any key to enter the REPL. Use CTRL-D to reload.\r\n");
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
            if (safe_mode == NO_SAFE_MODE) {
                new_status_color(color_brightness(ALL_DONE, brightness));
            } else {
                new_status_color(color_brightness(SAFE_MODE, brightness));
            }
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

// Provided by the linker;
extern uint32_t _ezero;

safe_mode_t samd21_init(void) {
#ifdef ENABLE_MICRO_TRACE_BUFFER
    REG_MTB_POSITION = ((uint32_t) (mtb - REG_MTB_BASE)) & 0xFFFFFFF8;
    REG_MTB_FLOW = (((uint32_t) mtb - REG_MTB_BASE) + TRACE_BUFFER_SIZE_BYTES) & 0xFFFFFFF8;
    REG_MTB_MASTER = 0x80000000 + (TRACE_BUFFER_MAGNITUDE_PACKETS - 1);
#endif

// On power on start or external reset, set _ezero to the canary word. If it
// gets killed, we boot in safe mod. _ezero is the boundary between statically
// allocated memory including the fixed MicroPython heap and the stack. If either
// misbehaves, the canary will not be in tact after soft reset.
#ifdef CIRCUITPY_CANARY_WORD
    if (PM->RCAUSE.bit.POR == 1 || PM->RCAUSE.bit.EXT == 1) {
        _ezero = CIRCUITPY_CANARY_WORD;
    } else if (PM->RCAUSE.bit.SYST == 1) {
        // If we're starting from a system reset we're likely coming from the
        // bootloader or hard fault handler. If we're coming from the handler
        // the canary will be CIRCUITPY_SAFE_RESTART_WORD and we don't want to
        // revive the canary so that a second hard fault won't restart. Resets
        // from anywhere else are ok.
        if (_ezero == CIRCUITPY_SAFE_RESTART_WORD) {
            _ezero = ~CIRCUITPY_CANARY_WORD;
        } else {
            _ezero = CIRCUITPY_CANARY_WORD;
        }
    }
#endif

    load_serial_number();

    irq_initialize_vectors();
    cpu_irq_enable();

    // Initialize the sleep manager
    sleepmgr_init();

    uint16_t dfll_fine_calibration = 0x1ff;
#ifdef CALIBRATE_CRYSTALLESS
    // This is stored in an NVM page after the text and data storage but before
    // the optional file system. The first 16 bytes are the identifier for the
    // section.
    if (strcmp((char*) INTERNAL_CIRCUITPY_CONFIG_START_ADDR, "CIRCUITPYTHON1") == 0) {
        dfll_fine_calibration = ((uint16_t *) INTERNAL_CIRCUITPY_CONFIG_START_ADDR)[8];
    }
#endif

    // We pass in the DFLL fine calibration because we can't change it once the
    // clock is going.
    system_init(dfll_fine_calibration);

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

    // Init the nvm controller.
    struct nvm_config config_nvm;
    nvm_get_config_defaults(&config_nvm);
    config_nvm.manual_page_write = false;
    nvm_set_config(&config_nvm);

    init_shared_dma();

    #ifdef CIRCUITPY_CANARY_WORD
    // Run in safe mode if the canary is corrupt.
    if (_ezero != CIRCUITPY_CANARY_WORD) {
        return HARD_CRASH;
    }
    #endif

    if (PM->RCAUSE.bit.BOD33 == 1 || PM->RCAUSE.bit.BOD12 == 1) {
        return BROWNOUT;
    }

    if (board_requests_safe_mode()) {
        return USER_SAFE_MODE;
    }

    #if CIRCUITPY_INTERNAL_NVM_SIZE > 0
    // Upgrade the nvm flash to include one sector for eeprom emulation.
    struct nvm_fusebits fuses;
    if (nvm_get_fuses(&fuses) == STATUS_OK &&
            fuses.eeprom_size == NVM_EEPROM_EMULATOR_SIZE_0) {
        #ifdef INTERNAL_FLASH_FS
        // Shift the internal file system up one row.
        for (uint8_t row = 0; row < TOTAL_INTERNAL_FLASH_SIZE / NVMCTRL_ROW_SIZE; row++) {
            uint32_t new_row_address = INTERNAL_FLASH_MEM_SEG1_START_ADDR + row * NVMCTRL_ROW_SIZE;
            nvm_erase_row(new_row_address);
            nvm_write_buffer(new_row_address,
                             (uint8_t*) (new_row_address + CIRCUITPY_INTERNAL_EEPROM_SIZE),
                             NVMCTRL_ROW_SIZE);
        }
        #endif
        uint32_t nvm_size = CIRCUITPY_INTERNAL_NVM_SIZE;
        uint8_t enum_value = 6;
        while (nvm_size > 256 && enum_value != 255) {
            nvm_size /= 2;
            enum_value -= 1;
        }
        if (enum_value != 255 && nvm_size == 256) {
            // Mark the last section as eeprom now.
            fuses.eeprom_size = (enum nvm_eeprom_emulator_size) enum_value;
            nvm_set_fuses(&fuses);
        }
    }
    #endif

    return NO_SAFE_MODE;
}

extern uint32_t _estack;
extern uint32_t _ebss;

int main(void) {
    // initialise the cpu and peripherals
    safe_mode_t safe_mode = samd21_init();

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    mp_stack_ctrl_init();
    mp_stack_set_limit((char*)&_estack - (char*)&_ebss - 1024);


#if MICROPY_MAX_STACK_USAGE
    // _ezero (same as _ebss) is an int, so start 4 bytes above it.
    mp_stack_set_bottom(&_ezero + 1);
    mp_stack_fill_with_sentinel();
#endif

    init_flash_fs();

    // Reset everything and prep MicroPython to run boot.py.
    reset_samd21();
    reset_board();
    reset_mp();

    // Turn on autoreload by default but before boot.py in case it wants to change it.
    autoreload_enable();

    // By default our internal flash is readonly to local python code and
    // writeable over USB. Set it here so that boot.py can change it.
    flash_set_usb_writeable(true);

    // If not in safe mode, run boot before initing USB and capture output in a
    // file.
    if (safe_mode == NO_SAFE_MODE && MP_STATE_VM(vfs_mount_table) != NULL) {
        new_status_color(BOOT_RUNNING);
        #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
        // Since USB isn't up yet we can cheat and let ourselves write the boot
        // output file.
        flash_set_usb_writeable(false);
        FIL file_pointer;
        boot_output_file = &file_pointer;
        f_open(&((fs_user_mount_t *) MP_STATE_VM(vfs_mount_table)->obj)->fatfs,
            boot_output_file, CIRCUITPY_BOOT_OUTPUT_FILE, FA_WRITE | FA_CREATE_ALWAYS);
        flash_set_usb_writeable(true);
        #endif

        // TODO(tannewt): Re-add support for flashing boot error output.
        bool found_boot = maybe_run("settings.txt", NULL) ||
                          maybe_run("settings.py", NULL) ||
                          maybe_run("boot.py", NULL) ||
                          maybe_run("boot.txt", NULL);
        (void) found_boot;

        #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
        f_close(boot_output_file);
        flash_flush();
        boot_output_file = NULL;
        #endif

        // Reset to remove any state that boot.py setup. It should only be used to
        // change internal state thats not in the heap.
        reset_samd21();
        reset_mp();
    }

    usb_hid_init();

    // Start USB after getting everything going.
    #ifdef USB_REPL
        udc_start();
    #endif

    // Boot script is finished, so now go into REPL/main mode.
    int exit_code = PYEXEC_FORCED_EXIT;
    bool skip_repl = true;
    bool first_run = true;
    for (;;) {
        if (!skip_repl) {
            // The REPL mode can change, or it can request a reload.
            bool autoreload_on = autoreload_is_enabled();
            autoreload_disable();
            new_status_color(REPL_RUNNING);
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                exit_code = pyexec_raw_repl();
            } else {
                exit_code = pyexec_friendly_repl();
            }
            if (autoreload_on) {
                autoreload_enable();
            }
            reset_samd21();
            reset_board();
            reset_mp();
        }
        if (exit_code == PYEXEC_FORCED_EXIT) {
            if (!first_run) {
                mp_hal_stdout_tx_str("soft reboot\r\n");
            }
            first_run = false;
            skip_repl = start_mp(safe_mode);
            reset_samd21();
            reset_board();
            reset_mp();
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
    // This collects root pointers from the VFS mount table. Some of them may
    // have lost their references in the VM even though they are mounted.
    gc_collect_root((void**)&MP_STATE_VM(vfs_mount_table), sizeof(mp_vfs_mount_t) / sizeof(mp_uint_t));
    // This naively collects all object references from an approximate stack
    // range.
    gc_collect_root(&dummy, ((mp_uint_t)&_estack - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

void NORETURN nlr_jump_fail(void *val) {
    HardFault_Handler();
    while (true) {}
}

void NORETURN __fatal_error(const char *msg) {
    HardFault_Handler();
    while (true) {}
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
