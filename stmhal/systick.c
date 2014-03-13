#include <stm32f4xx_hal.h>
#include "misc.h"
#include "systick.h"

void sys_tick_init(void) {
    // SysTick_Config is now called from HAL_RCC_ClockConfig, which is called
    // from SystemClock_Config

    // SysTick_Config sets the SysTick_IRQn to be the lowest priority, but
    // we want it to be the highest priority, so fix things here.
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void sys_tick_delay_ms(uint32_t delay_ms) {
    sys_tick_wait_at_least(HAL_GetTick(), delay_ms);
}

// waits until at least delay_ms milliseconds have passed from the sampling of stc
// handles overflow properl
// assumes stc was taken from HAL_GetTick() some time before calling this function
// eg stc <= HAL_GetTick() for the case of no wrap around of HAL_GetTick()
void sys_tick_wait_at_least(uint32_t stc, uint32_t delay_ms) {
    // stc_wait is the value of HAL_GetTick() that we wait for
    uint32_t stc_wait = stc + delay_ms;
    if (stc_wait < stc) {
        // stc_wait wrapped around
        while (stc <= HAL_GetTick() || HAL_GetTick() < stc_wait) {
            __WFI(); // enter sleep mode, waiting for interrupt
        }
    } else {
        // stc_wait did not wrap around
        while (stc <= HAL_GetTick() && HAL_GetTick() < stc_wait) {
            __WFI(); // enter sleep mode, waiting for interrupt
        }
    }
}

bool sys_tick_has_passed(uint32_t stc, uint32_t delay_ms) {
    // stc_wait is the value of HAL_GetTick() that we wait for
    uint32_t stc_wait = stc + delay_ms;
    if (stc_wait < stc) {
        // stc_wait wrapped around
        return !(stc <= HAL_GetTick() || HAL_GetTick() < stc_wait);
    } else {
        // stc_wait did not wrap around
        return !(stc <= HAL_GetTick() && HAL_GetTick() < stc_wait);
    }
}
