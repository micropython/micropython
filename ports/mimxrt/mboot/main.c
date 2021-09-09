/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
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
 *
 */

/*
  * After device is enumerated in dfu mode run the following commands
  *
  * To transfer firmware from host to device:
  *
  * $ dfu-util -D [filename]
  *
  * To transfer firmware from device to host:
  *
  * $ dfu-util -U [filename]
  *
  */

#include CPU_HEADER_H
// include fsl_clock.h BEFORE including clock configuration due to bug in NXP HAL.
#include "fsl_clock.h"
#include CLOCK_CONFIG_H
#include <stdlib.h>
#include "mboot_buffer.h"
#include "mboot_command.h"
#include "mboot_dfu.h"
#include "mboot_memory.h"
#include "mboot_validate.h"
#include "fw_header.h"

// --------------------------------------------------------------------+
// Type Definitions
// --------------------------------------------------------------------+
typedef void (*firmware_entry_func)(void);

// --------------------------------------------------------------------+
// Local Variables
// --------------------------------------------------------------------+
MEM_AT_SECTION("._bl_command") mboot_command_t bl_command;
const uint8_t dcd_data[] = { 0x00 };

// --------------------------------------------------------------------+
// External Variables
// --------------------------------------------------------------------+
extern uint32_t __firmware_start;  // Linker symbol

// --------------------------------------------------------------------+
// External Function Declarations
// --------------------------------------------------------------------+
extern void SystemInit();

// --------------------------------------------------------------------+
// Local Function Declarations
// --------------------------------------------------------------------+
static void board_init(void);
static bool reprog_request(void);
static bool valid_fw_available(void);
static inline firmware_entry_func get_entry_func(void);
static inline void fw_start(void) __attribute__ ((noreturn));

// --------------------------------------------------------------------+
// Global Function Definitions
// --------------------------------------------------------------------+
int main(void) {
    board_init();

    if (reprog_request() || !valid_fw_available()) {
        mboot_dfu_init();
        mboot_dfu_run();
        // Reset controller after upgrade
        NVIC_SystemReset();
    } else {
        fw_start();
    }

    // Fatal error occured - wait for power cycle
    for (;;)
    {
        __asm__ ("nop");
    }

    return 0;
}

// --------------------------------------------------------------------+
// Local Function Definitions
// --------------------------------------------------------------------+
static bool reprog_request() {
    bool reprogramming_requested = false;

    if ((bl_command.magic == BOOT_COMMAND_MAGIC) && (bl_command.command == BOOT_COMMAND_RESET_DFU)) {
        reprogramming_requested = true;
    } else {
        reprogramming_requested = false;
    }

    // Clear reprogramming header flags immediately to prevent reboot loops
    bl_command.magic = 0x0UL;
    bl_command.command = 0x0UL;

    return reprogramming_requested;
}

static bool valid_fw_available() {
    fw_header_t *fw_header = (fw_header_t *)MEM_GET_SYMBOL_VALUE(__firmware_start);
    uintptr_t start_addr = ((uintptr_t)MEM_GET_SYMBOL_VALUE(__firmware_start)) + sizeof(fw_header_t);
    mboot_validate_status_t fw_status = mboot_validate_firmware(fw_header, start_addr);

    return fw_status == FWV_STATUS_OK ? true : false;
}

static inline firmware_entry_func get_entry_func() {
    fw_header_t *fw_header = (fw_header_t *)&__firmware_start;
    firmware_entry_func entry_func = (firmware_entry_func)fw_header->entry_addr;
    return entry_func;
}

static inline void fw_start() {
    firmware_entry_func entry_func = get_entry_func();

    entry_func();

    for (;;)
    {
        __asm__ ("nop");
    }
}

static void board_init() {
    // Init clock
    BOARD_BootClockRUN();
    SystemCoreClockUpdate();

    // Enable IOCON clock
    CLOCK_EnableClock(kCLOCK_Iomuxc);
}
