/*
 * GD32F4xx MicroPython port - main entry
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/cstack.h"
#include "py/pystack.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"
#if MICROPY_PY_NETWORK
#include "extmod/modnetwork.h"
#endif
#if MICROPY_PY_LWIP
#include "lwip/init.h"
#endif

#include "gd32f4xx.h"
#include "board.h"

void machine_mem_backup_init(void);
#include "mphalport.h"

// Drain the REPL UART and leave its TX pin driven high, so a following
// NVIC_SystemReset() does not emit a spurious byte on the wire.
extern void usart_repl_shutdown(void);

// RTC backup-register hand-off of the REPL mode across the reset
// (implemented in machine_mem_backup_port.c).
extern void machine_repl_mode_save(int mode);
extern int machine_repl_mode_restore(void);

// memory
static char *stack_top;
static char heap[MICROPY_HEAP_SIZE];

// Buffer for MicroPython's Python-level stack (push/pop of VM frames).
// mp_state_ctx is in .bss, so without mp_pystack_init() the start/end
// pointers are NULL and the first allocation reports "pystack exhausted".
#if MICROPY_ENABLE_PYSTACK
static uint8_t pystack[MICROPY_PYSTACK_SIZE];
#endif

extern uint32_t SystemCoreClock;

// Provided by the linker script / Makefile:  _sp is the top of the
// C stack, __stack_size its size (see LDFLAGS --defsym).
extern uint8_t _sp;
extern uint8_t __stack_size;
extern void uart_repl_init(void);

// board init hooks (weak defaults)
MP_WEAK void board_early_init(void) {
}

MP_WEAK void board_init(void) {
}

// GC
void gc_collect(void) {
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

// filesystem support: mount littlefs2 on internal flash at "/"
extern mp_obj_t pyb_flash_get_block_device(void);
extern bool flash_bdev_is_blank(void);

// Set in main() before mp_init(): the blank scan is pure memory reads
// and needs no Python machinery.
static bool fs_was_blank;

// Format the littlefs partition in place.  Only called when the region is
// completely blank, so it cannot destroy an existing filesystem.
static int format_internal_fs(void) {
    int ret = -MP_EIO;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // VfsLfs2 lives in the `vfs` module, not in the globals namespace.
        mp_obj_t vfs_mod = mp_import_name(MP_QSTR_vfs, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
        mp_obj_t vfs_cls = mp_load_attr(vfs_mod, MP_QSTR_VfsLfs2);
        mp_obj_t mkfs = mp_load_attr(vfs_cls, MP_QSTR_mkfs);
        mp_call_function_1(mkfs, pyb_flash_get_block_device());
        ret = 0;
        nlr_pop();
    }
    return ret;
}

// Mount the internal filesystem at "/".  Returns 0 on success.
//
// A blank partition is formatted first: constructing VfsLfs2 mounts
// immediately, so formatting has to happen before that call.  We only format
// when every byte of the region is 0xFF, so an existing filesystem is never
// touched.
static void write_default_boot_py(void);

static int mount_internal_fs(void) {
    int ret = -MP_EIO;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t bdev = pyb_flash_get_block_device();

        if (fs_was_blank) {
            format_internal_fs();
        }

        // Build VfsLfs2 explicitly.  Handing the raw block device to
        // mp_vfs_mount() would go through mp_vfs_autodetect(), which looks for a
        // "littlefs" signature and reports a misleading ENODEV on failure.
        //
        // readsize/progsize are left at the 32-byte default.  Raising them would
        // lift littlefs' inline-file limit from 128 B to 1 KB
        // (cache_size = MIN(block_size, 4 * MAX(read_size, prog_size))), which
        // sounds attractive for small scripts, but cache_size is allocated per
        // open file and this board only has ~15 KB of spare RAM.  The 4 KB block
        // size alone already lifts the partition from 3 usable blocks to 12.
        mp_obj_t vfs_mod = mp_import_name(MP_QSTR_vfs, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
        mp_obj_t vfs_cls = mp_load_attr(vfs_mod, MP_QSTR_VfsLfs2);
        mp_obj_t lfs2 = mp_call_function_1(vfs_cls, bdev);
        ret = mp_vfs_mount_and_chdir_protected(lfs2, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));

        // Create a starter /boot.py if the filesystem does not have one yet.
        // Only ever on a blank or freshly formatted partition: an existing file
        // is the user's and must never be overwritten.
        if (ret == 0 && fs_was_blank) {
            write_default_boot_py();
        }
        nlr_pop();
    }
    return ret;
}

// Starter /boot.py written on a blank filesystem.
//
// Kept well under the 128-byte inline threshold where practical, but the
// explanatory comments are worth the single 4 KB block they cost: this file is
// the first thing a user reads.  Every statement is commented out so the
// default boot sequence is unchanged until the user edits it.
static const char default_boot_py[] =
    "# boot.py - runs at every reset, before main.py.\n"
    "#\n"
    "# Edit this file over the REPL or with mpremote; no reflashing needed.\n"
    "# Keep it short: it runs before the REPL and before main.py.\n"
    "#\n"
    "# If this file raises, the traceback is printed and main.py still runs,\n"
    "# so a broken boot.py cannot leave the board unreachable.\n"
    "#\n"
    "# Example: mount the SPI flash as /flash before the app starts.\n"
    "#   import machine, os, vfs, w25q_flash\n"
    "#   os.mount(vfs.VfsLfs2(w25q_flash.W25Q()), '/flash')\n"
    "\n"
    "import gc\n"
    "gc.collect()\n";

// Write default_boot_py[] to /boot.py.  Called only when the filesystem was
// blank, i.e. the region had never been used, so there is nothing to lose.
//
// Uses the C string "boot.py" rather than a qstr: qstr pool entries depend on
// whatever the build happened to scan, and a missing MP_QSTR_boot_dot_py would
// be a compile error deep inside a rarely-exercised branch.
static void write_default_boot_py(void) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t io_mod = mp_import_name(MP_QSTR_io, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
        mp_obj_t open_fn = mp_load_attr(io_mod, MP_QSTR_open);
        mp_obj_t path = mp_obj_new_str("boot.py", 7);
        mp_obj_t mode = mp_obj_new_str("w", 1);
        mp_obj_t args[2] = { path, mode };
        mp_obj_t f = mp_call_function_n_kw(open_fn, 2, 0, args);
        mp_obj_t data = mp_obj_new_str(default_boot_py, sizeof(default_boot_py) - 1);
        mp_obj_t write_fn = mp_load_attr(f, MP_QSTR_write);
        mp_call_function_1(write_fn, data);
        mp_obj_t close_fn = mp_load_attr(f, MP_QSTR_close);
        mp_call_function_0(close_fn);
        nlr_pop();
    }
    // An unwritable filesystem is not fatal: boot just continues without a
    // starter file, and the REPL is still reachable.
}

void NORETURN __fatal_error(const char *msg) {
    (void)msg;
    for (;;) {
        ;
    }
}

void nlr_jump_fail(void *val) {
    (void)val;
    __fatal_error("FATAL: nlr_jump_fail");
}

int main(void) {
    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    // enable DWT cycle counter for microsecond delays
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // SysTick at 1kHz
    SysTick_Config(SystemCoreClock / 1000);

    board_early_init();
    uart_repl_init();

    // NOTE: the "GD32F407 with GD32F407VE" banner is deliberately NOT printed
    // here.  mpremote's enter_raw_repl() soft-resets the board and then waits
    // for the output to END with "soft reboot\r\n".  Anything printed after
    // that line - including a banner - makes the wait time out and the tool
    // reports "could not enter raw repl".  The banner is emitted just before
    // the soft-reboot message instead; see the reset path at the end of main().
    // Tell MicroPython where the C stack is and how big, so that
    // mp_cstack_check() can convert an imminent overflow into a Python
    // RecursionError instead of a hard fault.  Without this the stack limit
    // stays 0 (BSS) and every stack check fires immediately.
    mp_cstack_init_with_top(&_sp, (size_t)&__stack_size);

    // Give MicroPython a real Python stack.  Without this the start/end
    // pointers stay NULL (they live in .bss) and the first allocation
    // raises "pystack exhausted".
    #if MICROPY_ENABLE_PYSTACK
    mp_pystack_init(pystack, pystack + sizeof(pystack));
    #endif

    // Scan for a blank partition before mp_init(): it is pure memory reads and
    // needs no Python objects, so doing it here keeps the mount path simple.
    fs_was_blank = flash_bdev_is_blank();

    gc_init(heap, heap + sizeof(heap));
    mp_init();

    // lwIP and the network module need explicit initialisation.  Nothing else in
    // this port did it, so the memp pools stayed zeroed and the first
    // socket.socket() failed with ENOMEM even though the link was up and the
    // interface had an address.  ports/stm32 calls these from main() as well;
    // lwIP only tolerates being initialised once per power-up because its
    // timeout list is only ever reset by BSS clearing.
    #if MICROPY_PY_LWIP
    lwip_init();
    #endif
    #if MICROPY_PY_NETWORK
    mod_network_init();
    #endif

    if (mount_internal_fs() != 0) {
        mp_printf(&mp_plat_print,
            "storage: '/' not mounted, run machine.Flash().mkfs()\r\n");
    }

    // Enable write access to the RTC backup domain.  Done before anything that
    // might read the hand-off register, and before Python code could touch it.
    machine_mem_backup_init();

    // A soft reset is a real NVIC_SystemReset() here, so pyexec_mode_kind does
    // not survive it.  Recover the mode the previous boot asked for; without
    // this the board would answer a tool's raw-REPL session with a friendly
    // banner and the tool would time out (see machine_mem_backup_port.c).
    int restored_mode = machine_repl_mode_restore();
    if (restored_mode >= 0) {
        pyexec_mode_kind = (pyexec_mode_kind_t)restored_mode;
    }

    // Run the frozen _boot.py (if present) so user boot code can run before
    // the REPL appears.  This matches what ports/stm32 and ports/samd do.
    #if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("_boot.py", false);
    #endif

    // Then the user-editable /boot.py from the filesystem, then /main.py -
    // the same order ports/stm32 and ports/rp2 use.  Both are no-ops when the
    // file is absent, so a freshly formatted filesystem is fine.
    //
    // /boot.py is the place for setup that must happen before the application:
    // initialising a display, mounting an external filesystem, configuring a
    // bus.  Keeping it in the filesystem (rather than frozen into the firmware)
    // means it can be edited over the REPL without reflashing.
    //
    // BOTH run even if boot.py raises.  That is deliberate, and it differs from
    // ports/rp2: there `ret != 0` gates main.py, which only works because rp2
    // enables MICROPY_PYEXEC_ENABLE_EXIT_CODE_HANDLING (normal exit == 0).
    // This port leaves it disabled, so the codes are inverted:
    // normal exit == 1, unhandled exception == 0.  Copying rp2's test here
    // would silently skip main.py on success and run it after a crash - the
    // exact opposite of the intent.  The traceback is already on the console;
    // refusing to start the application on top of it would leave the board
    // looking dead with no prompt.
    //
    // Both are skipped when the previous boot handed over a raw-REPL session:
    // that request comes from a tool (mpremote, Thonny) which wants the REPL,
    // not the user's application, and running user code first would block it.
    int main_ret = 0;
    if (pyexec_mode_kind != PYEXEC_MODE_RAW_REPL) {
        int boot_ret = pyexec_file_if_exists("boot.py");

        // A soft reset requested from boot.py skips main.py entirely.
        if (!(boot_ret & PYEXEC_FORCED_EXIT)) {
            main_ret = pyexec_file_if_exists("main.py");
        } else {
            main_ret = boot_ret;
        }
    }

    board_init();

    // A finished, interrupted or failed main.py is not itself a reason to
    // return to Python without the user asking; but a soft reset requested from
    // inside it must be honoured without starting a REPL first.
    if (!(main_ret & PYEXEC_FORCED_EXIT)) {
        // Both REPL functions contain their own loop and only return on a soft
        // reset (PYEXEC_FORCED_EXIT) or when the requested mode changes
        // (return 0).  Dispatch on pyexec_mode_kind and re-enter, resetting the
        // MCU only when a reset was actually asked for.  Resetting on every
        // return would print the banner into the middle of a raw-REPL session,
        // which Thonny reports as "Command output was not empty".
        for (;;) {
            int ret;
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                ret = pyexec_raw_repl();
            } else {
                ret = pyexec_friendly_repl();
            }

            if (ret & PYEXEC_FORCED_EXIT) {
                break;
            }
            // Otherwise the REPL asked to switch mode; loop and dispatch again.
        }
    }

    // Remember which REPL mode the next boot should come up in.  A tool that is
    // driving a raw-REPL session issues Ctrl-D expecting raw REPL back; if we
    // lost that here it would see a friendly banner and give up.
    //
    // Saved BEFORE mp_deinit() so the mode is still valid and the backup domain
    // is still accessible.
    int next_mode = (int)pyexec_mode_kind;

    mp_deinit();

    machine_repl_mode_save(next_mode);

    // Print the banner, then announce the soft reset.  The order is what
    // mpremote's enter_raw_repl() depends on: it waits for output ending in
    // "soft reboot\r\n", so that string must be the LAST thing on the wire
    // before the fresh REPL starts talking.  ports/stm32 and ports/rp2 emit the
    // same marker, but there the banner comes from the REPL itself and so
    // naturally lands before it.
    //
    // In raw mode the banner is suppressed: a raw-REPL client expects machine
    // readable output only, and ports/stm32/samd behave the same way.
    if (pyexec_mode_kind != PYEXEC_MODE_RAW_REPL) {
        mp_hal_stdout_tx_str(MICROPY_HW_BOARD_NAME);
        mp_hal_stdout_tx_str(" with ");
        mp_hal_stdout_tx_str(MICROPY_HW_MCU_NAME);
        mp_hal_stdout_tx_str("\r\n");
    }
    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");

    // Silence the REPL UART before resetting.
    //
    // NVIC_SystemReset() pulls every GPIO back to its reset state, so PA9 stops
    // being driven high for a few microseconds.  The USB-serial adapter sees
    // that as a start bit followed by idle-high data and hands the host a stray
    // 0xFF, which Thonny renders as a mojibake glyph right after
    // "%Run -c $EDITOR_CONTENT".  A hardware reset triggered from the debugger
    // does not show it because the pin is already high before the core restarts.
    //
    // Wait for the transmitter to drain, then hold the pin high as a plain
    // GPIO output across the reset; that keeps the line idle-high the whole
    // time instead of letting it float.
    usart_repl_shutdown();

    NVIC_SystemReset();
}
