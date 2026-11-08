// machine_mem_backup_port.c - machine.mem_backup() region table for GD32F4xx
//
// extmod/machine_mem.c includes this file when MICROPY_PY_MACHINE_MEM_BACKUP is
// enabled and expects a machine_mem_backup_regions[] table.  The GD32F4 has 20
// RTC backup registers (RTC_BKP0..RTC_BKP19), each 32 bits, for 80 bytes total.
// They are retained as long as VBAT (or the main supply) stays up, including
// across reset and deep-sleep.
//
// Writing to the backup domain requires the PMU backup write enable bit, which
// pmu_backup_write_enable() sets.
//
// RTC_BKP19 is reserved by the port and NOT exposed to Python: it carries the
// REPL mode across a soft reset (see "REPL mode hand-off" below).

#include "gd32f4xx.h"
#include "shared/runtime/pyexec.h"

// One register is reserved for the port; expose the other 19 to Python so that
// mem_backup() cannot clobber the port's own state.
#define GD32_BKP_REG_COUNT      (20)
#define GD32_BKP_EXPOSED_COUNT  (19)

static const mp_obj_array_t machine_mem_backup_regions[] = {
    BACKUP_MV('I', GD32_BKP_EXPOSED_COUNT, (void *)&RTC_BKP0),
};

// Enable write access to the backup domain.  Called once during port init.
void machine_mem_backup_init(void) {
    rcu_periph_clock_enable(RCU_BKPSRAM);   // backup domain clock
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
}

// ---------------------------------------------------------------------------
// REPL mode hand-off across a soft reset
// ---------------------------------------------------------------------------
// This port implements "soft reset" as a real NVIC_SystemReset(), which clears
// RAM and therefore loses pyexec_mode_kind.  tools such as mpremote and Thonny
// depend on the opposite behaviour: they interrupt the running program, enter
// the raw REPL, request a soft reset, and then expect the board to come back
// still talking the raw-REPL protocol.  With the mode reset to "friendly" the
// board prints a normal banner instead and those tools time out with
// "could not enter raw repl".
//
// ports/stm32 does not have this problem because its soft reset re-enters a
// loop inside the same process, so pyexec_mode_kind simply survives.
//
// Keep the requested mode in a backup register, which the reset does not clear.
//
// Layout: high 24 bits = tag, low 8 bits = mode.  Splitting them this way keeps
// the validity test trivial and immune to the arithmetic mistakes a XOR-packed
// value invites.
#define REPL_MODE_TAG_SHIFT (8)
#define REPL_MODE_TAG       (0x524550u)     // 'REP'
#define REPL_MODE_MASK      (0xFFu)
#define RTC_BKP_REPL_MODE   (RTC_BKP19)

// Called just before NVIC_SystemReset().
void machine_repl_mode_save(int mode) {
    RTC_BKP_REPL_MODE = (REPL_MODE_TAG << REPL_MODE_TAG_SHIFT) | ((uint32_t)mode & REPL_MODE_MASK);
}

// Called early in main(), after the backup domain is writable.  Returns the
// mode requested by the previous reset, or -1 when there was none.
int machine_repl_mode_restore(void) {
    uint32_t raw = RTC_BKP_REPL_MODE;
    int mode = -1;

    if ((raw >> REPL_MODE_TAG_SHIFT) == REPL_MODE_TAG) {
        mode = (int)(raw & REPL_MODE_MASK);
        if (mode != PYEXEC_MODE_FRIENDLY_REPL && mode != PYEXEC_MODE_RAW_REPL) {
            mode = -1;
        }
    }

    // Clear it so a plain power-on or debugger reset starts fresh.
    RTC_BKP_REPL_MODE = 0;
    return mode;
}
