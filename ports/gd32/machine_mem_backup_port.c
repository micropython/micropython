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

#include "gd32f4xx.h"

#define GD32_BKP_REG_COUNT  (20)

static const mp_obj_array_t machine_mem_backup_regions[] = {
    BACKUP_MV('I', GD32_BKP_REG_COUNT, (void *)&RTC_BKP0),
};

// Enable write access to the backup domain.  Called once during port init.
void machine_mem_backup_init(void) {
    rcu_periph_clock_enable(RCU_BKPSRAM);   // backup domain clock
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
}
