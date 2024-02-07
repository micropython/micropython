#include "py/mpconfig.h"
#include "py/mphal.h"
#include "FreeRTOS.h"
#include "task.h"

#if MICROPY_PY_MACHINE_BITSTREAM

#define NS_TICKS_OVERHEAD (0) // set overhead as required to adjust the level

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {

    // for register level
    // uint32_t outMask = CY_GPIO_OUT_MASK << pinNum;
    // uint32_t outMask = CY_GPIO_OUT_MASK << P13_6_NUM;

    GPIO_PRT_Type *base = CYHAL_GET_PORTADDR(pin);
    uint32_t pinNum = CYHAL_GET_PIN(pin);

    uint32_t fcpu_mhz = mp_hal_get_cpu_freq() / 1000000;

    mp_printf(&mp_plat_print, "freq%lu", fcpu_mhz);

    // Convert ns to cycles [high_time_0, low_time_0, high_time_1, low_time_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
        // if (timing_ns[i] > NS_TICKS_OVERHEAD) {
        //     timing_ns[i] -= NS_TICKS_OVERHEAD;
        // }
        // if (i % 2 == 1) {
        //     // Convert low_time to period (i.e. add high_time).
        //     timing_ns[i] += timing_ns[i - 1];
        // }
    }

    // uint32_t irq_state = portSET_INTERRUPT_MASK_FROM_ISR();
    taskENTER_CRITICAL();          // FreeRTOS function for critical section
    // uint32_t irq_state = cyhal_system_critical_section_enter();  //hal function for critical section

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            // cyhal_gpio_write(P13_6, true);           // HAL takes 1 us to execute here
            // cyhal_gpio_write(P13_6, true);
            // Cy_GPIO_Write(base, pinNum, 1);          // PDL takes 320 ns to execute
            // Cy_GPIO_Write(base, pinNum, 0);
            // GPIO_PRT_OUT_SET(P13_6_PORT) = outMask;    // Register access takes 160 ns but not stable
            // GPIO_PRT_OUT_CLR(P13_6_PORT) = outMask;
            uint32_t *t = &timing_ns[b >> 6 & 2];
            Cy_GPIO_Write(base, pinNum, 1);
            Cy_SysLib_DelayCycles(t[0]);
            Cy_GPIO_Write(base, pinNum, 0);
            Cy_SysLib_DelayCycles(t[1]);
            b <<= 1;
        }
    }
// cyhal_system_critical_section_exit(irq_state);
// portCLEAR_INTERRUPT_MASK_FROM_ISR(irq_state);
    taskEXIT_CRITICAL();

}

#endif // MICROPY_PY_MACHINE_BITSTREAM
