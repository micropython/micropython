#include "py/mpconfig.h"
#include "py/mphal.h"
#include "FreeRTOS.h"
#include "task.h"

#if MICROPY_PY_MACHINE_BITSTREAM

#define NS_TICKS_OVERHEAD (0) // set overhead as required to adjust the level
uint32_t range = 0; // range<1000 ns

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {

    GPIO_PRT_Type *base = CYHAL_GET_PORTADDR(pin);
    uint32_t pinNum = CYHAL_GET_PIN(pin);
    uint32_t fcpu_mhz = mp_hal_get_cpu_freq() / 1000000;

    cyhal_gpio_init(pin, CYHAL_GPIO_DIR_OUTPUT,
        CYHAL_GPIO_DRIVE_STRONG, 1);

    // Convert ns to cycles [high_time_0, low_time_0, high_time_1, low_time_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
    }

    // loop to check the timing values range
    for (size_t i = 0; i < 4; ++i) {
        if ((timing_ns[i]) >= 100) {
            range = 1;
        }
    }

    taskENTER_CRITICAL();          // FreeRTOS function for critical section

    switch (range)
    {
        case 0:
            for (size_t i = 0; i < len; i++)
            {
                uint8_t b = buf[i];
                // Send each bit of the byte
                for (size_t j = 0; j < 8; j++)
                {
                    bool bit = (b & 0x80) != 0;
                    b <<= 1;
                    switch (bit)
                    {
                        // Send a 1-bit
                        case 1:
                            // sendOneBit();
                            Cy_GPIO_Write(base, pinNum, 1);
                            Cy_GPIO_Write(base, pinNum, 1);
                            Cy_GPIO_Write(base, pinNum, 0);
                            break;
                        // Send a 0-bit
                        case 0:
                            // sendZeroBit();
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 1);
                            break;
                    }
                }
            }

            break;
        default:
            for (size_t i = 0; i < len; ++i) {
                uint8_t b = buf[i];
                for (size_t j = 0; j < 8; ++j) {
                    uint32_t *t = &timing_ns[b >> 6 & 2];
                    Cy_GPIO_Write(base, pinNum, 1);
                    Cy_SysLib_DelayCycles(t[0]);
                    Cy_GPIO_Write(base, pinNum, 0);
                    Cy_SysLib_DelayCycles(t[1]);
                    b <<= 1;
                }
            }
            break;
    }
    taskEXIT_CRITICAL();

}

#endif // MICROPY_PY_MACHINE_BITSTREAM
