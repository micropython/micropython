/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

#if MICROPY_PY_MACHINE_BITSTREAM

const uint32_t timing_sequence_1[] = {500, 1125, 800, 750};
uint32_t range = 0;

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {

    GPIO_PRT_Type *base = CYHAL_GET_PORTADDR(pin);
    uint32_t pinNum = CYHAL_GET_PIN(pin);
    uint32_t fcpu_mhz = mp_hal_get_cpu_freq() / 1000000;

    if (memcmp(timing_ns, timing_sequence_1,  4 * sizeof(uint32_t)) == 0) {
        range = 0;
    } else {
        for (size_t i = 0; i < 4; ++i) {
            if ((timing_ns[i]) <= 150) {
                mp_raise_ValueError(MP_ERROR_TEXT("Timing is not supported"));
            }
        }
        range = 1;
    }

    // Convert ns to cycles [high_time_0, low_time_0, high_time_1, low_time_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
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
                            Cy_GPIO_Write(base, pinNum, 1);
                            Cy_GPIO_Write(base, pinNum, 1);
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 0);
                            break;
                        // Send a 0-bit
                        case 0:
                            // sendZeroBit();
                            Cy_GPIO_Write(base, pinNum, 1);
                            Cy_GPIO_Write(base, pinNum, 1);
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 0);
                            Cy_GPIO_Write(base, pinNum, 0);
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
