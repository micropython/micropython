#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/rtc/__init__.h"
#include "nrf_wdt.h"

#define WDT_RELOAD_COUNT 2

void common_hal_wdt_init(uint32_t duration, bool pause_during_sleep) {
    unsigned int channel;
    nrf_wdt_behaviour_t behaviour = NRF_WDT_BEHAVIOUR_RUN_SLEEP_HALT;
    if (pause_during_sleep) {
        behaviour = NRF_WDT_BEHAVIOUR_PAUSE_SLEEP_HALT;
    }

    nrf_wdt_behaviour_set(NRF_WDT, behaviour);

    uint64_t ticks = (duration * 32768ULL) / 1000;
    if (ticks > UINT32_MAX) {
        mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
    }
    nrf_wdt_reload_value_set(NRF_WDT, (uint32_t) ticks);

    for (channel = 0; channel < WDT_RELOAD_COUNT; channel++) {
        nrf_wdt_reload_request_enable(NRF_WDT, channel);
    }

    nrf_wdt_task_trigger(NRF_WDT, NRF_WDT_TASK_START);
}

void common_hal_wdt_feed(void) {
    unsigned int channel;
    for (channel = 0; channel < WDT_RELOAD_COUNT; channel++) {
        nrf_wdt_reload_request_set(NRF_WDT, (nrf_wdt_rr_register_t)(NRF_WDT_RR0 + channel));
    }
}

void common_hal_wdt_disable(void) {
    // mp_raise_ValueError(translate("Watchdog timer cannot be disabled -- board will reset shortly"));
}
