// machine_bitstream_port.c - machine.bitstream implementation for GD32F4xx
//
// Uses the DWT cycle counter (Cortex-M4) to generate precisely timed bit
// streams, following the same approach as ports/stm32.  The GD32 GPIO "bit
// operate" register (GPIO_BOP) lets us change a pin in a single store, which
// is what makes the tight timing loop possible.

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

#if MICROPY_PY_MACHINE_BITSTREAM

// Measured overhead of the loop bookkeeping, in CPU cycles.
#define NS_CYCLES_OVERHEAD (6)

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    uint8_t port_idx = PIN_PORT(pin.pin);
    uint32_t pin_mask = (uint32_t)(1U << PIN_NUM(pin.pin));
    volatile uint32_t *bop = &GPIO_BOP(pin_port_base(port_idx));

    // Convert ns to CPU cycles: [high_0, low_0, high_1, low_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = SystemCoreClock / 1000000 * timing_ns[i] / 1000;
        if (timing_ns[i] > NS_CYCLES_OVERHEAD) {
            timing_ns[i] -= NS_CYCLES_OVERHEAD;
        }
        // The "low" entry is stored as an absolute deadline from the start of
        // the bit, so fold the preceding high time into it.
        if (i % 2 == 1) {
            timing_ns[i] += timing_ns[i - 1];
        }
    }

    mp_hal_ticks_cpu_enable();

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            DWT->CYCCNT = 0;
            *bop = pin_mask;                    // drive high
            uint32_t *t = &timing_ns[b >> 6 & 2];
            while (DWT->CYCCNT < t[0]) {
                ;
            }
            *bop = pin_mask << 16;              // drive low (BOP upper half clears)
            b <<= 1;
            while (DWT->CYCCNT < t[1]) {
                ;
            }
        }
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
