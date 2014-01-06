#include <stm32f4xx.h>
#include "misc.h"
#include "systick.h"

volatile uint32_t sys_tick_counter;

void sys_tick_init(void) {
    // sys-tick interrupt called at 1ms intervals
    sys_tick_counter = 0;
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, 0); // make it highest priority
}

// called on SysTick interrupt
void SysTick_Handler(void) {
    sys_tick_counter++;
    // hack!
    //void audio_drain(void);
    //audio_drain();
}

void sys_tick_delay_ms(uint32_t delay_ms) {
    sys_tick_wait_at_least(sys_tick_counter, delay_ms);
}

// waits until at least delay_ms milliseconds have passed from the sampling of sys_tick_counter in stc
// handles overflow properl
// assumes stc was taken from sys_tick_counter some time before calling this function
// eg stc <= sys_tick_counter for the case of no wrap around of sys_tick_counter
void sys_tick_wait_at_least(uint32_t stc, uint32_t delay_ms) {
    // stc_wait is the value of sys_tick_counter that we wait for
    uint32_t stc_wait = stc + delay_ms;
    if (stc_wait < stc) {
        // stc_wait wrapped around
        while (stc <= sys_tick_counter || sys_tick_counter < stc_wait) {
            __WFI(); // enter sleep mode, waiting for interrupt
        }
    } else {
        // stc_wait did not wrap around
        while (stc <= sys_tick_counter && sys_tick_counter < stc_wait) {
            __WFI(); // enter sleep mode, waiting for interrupt
        }
    }
}

bool sys_tick_has_passed(uint32_t stc, uint32_t delay_ms) {
    // stc_wait is the value of sys_tick_counter that we wait for
    uint32_t stc_wait = stc + delay_ms;
    if (stc_wait < stc) {
        // stc_wait wrapped around
        return !(stc <= sys_tick_counter || sys_tick_counter < stc_wait);
    } else {
        // stc_wait did not wrap around
        return !(stc <= sys_tick_counter && sys_tick_counter < stc_wait);
    }
}
