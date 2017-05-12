#include "autoreload.h"

#include "tick.h"

#include "asf/sam0/drivers/tc/tc_interrupt.h"

// Global millisecond tick count
volatile uint64_t ticks_ms = 0;

struct tc_module ms_timer;

static void ms_tick(struct tc_module *const module_inst) {
    // SysTick interrupt handler called when the SysTick timer reaches zero
    // (every millisecond).
    ticks_ms += 1;

    #ifdef CIRCUITPY_AUTORELOAD_DELAY_MS
        autoreload_tick();
    #endif
}

void tick_init() {
    struct tc_config config_tc;
    tc_get_config_defaults(&config_tc);
    config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
    config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
    config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    tc_init(&ms_timer, TC5, &config_tc);

    tc_set_top_value(&ms_timer, system_cpu_clock_get_hz() / 1000 - 1);
    tc_enable(&ms_timer);
    tc_register_callback(&ms_timer, ms_tick, TC_CALLBACK_OVERFLOW);
    tc_enable_callback(&ms_timer, TC_CALLBACK_OVERFLOW);
    tc_start_counter(&ms_timer);
}
