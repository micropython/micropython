#ifndef MICROPY_INCLUDED_PSOC6_SYSTEM_H
#define MICROPY_INCLUDED_PSOC6_SYSTEM_H

// defines how strong the hash for enable/disable interrupt is, how many chars long
#define HASH_CHARS_NUM 10

// function to return clock freq; Fast CLK (CM4) is the main sys clk
uint32_t system_get_cpu_freq(void);

// function to return clock freq divider for Fast CLK (CM4)
uint8_t system_get_cpu_freq_div(void);


// function to return 64-bit silicon ID of given PSoC microcontroller
// A combined 64-bit unique ID. [63:57] - DIE_YEAR [56:56] - DIE_MINOR [55:48] - DIE_SORT [47:40] - DIE_Y [39:32] - DIE_X [31:24] - DIE_WAFER [23:16] - DIE_LOT[2] [15: 8] - DIE_LOT[1] [ 7: 0] - DIE_LOT[0]
uint64_t system_get_unique_id(void);

// using watchdog timer to count to minimum value (1ms) to trigger reset
// thread-safe way as other methods might interfere with pending interrupts, threads etc.
void system_reset(void);

// get reset cause of the last system reset
// macros defined here: cy_syslib.h
uint32_t system_reset_cause(void);

// helper function to generate random alpha-numeric hash
uint8_t system_rand_hash(uint8_t length);

// function to disable global IRQs
// returns alpha-numeric hash to enable IRQs later
// see: https://docs.zephyrproject.org/apidoc/latest/group__isr__apis.html#ga19fdde73c3b02fcca6cf1d1e67631228
uint8_t system_disable_global_irq(void);
// function to enable global IRQs
// uses passed alpha-numeric key to verify and enable IRQs
bool system_enable_global_irq(uint8_t state);

#endif // MICROPY_INCLUDED_PSOC6_PIN_H
