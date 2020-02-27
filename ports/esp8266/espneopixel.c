// Original version from https://github.com/adafruit/Adafruit_NeoPixel
// Modifications by dpgeorge to support auto-CPU-frequency detection

// This is a mash-up of the Due show() code + insights from Michael Miller's
// ESP8266 work for the NeoPixelBus library: github.com/Makuna/NeoPixelBus
// Needs to be a separate .c file to enforce ICACHE_RAM_ATTR execution.

#include "py/mpconfig.h"
#if MICROPY_ESP8266_NEOPIXEL

#include "c_types.h"
#include "eagle_soc.h"
#include "user_interface.h"
#include "espneopixel.h"
#include "esp_mphal.h"

#define NEO_KHZ400 (1)

void /*ICACHE_RAM_ATTR*/ esp_neopixel_write(uint8_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz) {

    uint8_t *p, *end, pix, mask;
    uint32_t t, time0, time1, period, c, startTime, pinMask;

    pinMask = 1 << pin;
    p = pixels;
    end = p + numBytes;
    pix = *p++;
    mask = 0x80;
    startTime = 0;

    uint32_t fcpu = system_get_cpu_freq() * 1000000;

    #ifdef NEO_KHZ400
    if (is800KHz) {
    #endif
    time0 = fcpu / 2857143;  // 0.35us
    time1 = fcpu / 1250000;  // 0.8us
    period = fcpu / 800000;  // 1.25us per bit
    #ifdef NEO_KHZ400
} else {   // 400 KHz bitstream
    time0 = fcpu / 2000000;  // 0.5uS
    time1 = fcpu / 833333;   // 1.2us
    period = fcpu / 400000;  // 2.5us per bit
}
    #endif

    uint32_t irq_state = mp_hal_quiet_timing_enter();
    for (t = time0;; t = time0) {
        if (pix & mask) {
            t = time1;                                    // Bit high duration
        }
        while (((c = mp_hal_ticks_cpu()) - startTime) < period) {
            ;                                               // Wait for bit start
        }
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);   // Set high
        startTime = c;                                    // Save start time
        while (((c = mp_hal_ticks_cpu()) - startTime) < t) {
            ;                                               // Wait high duration
        }
        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);   // Set low
        if (!(mask >>= 1)) {                              // Next bit/byte
            if (p >= end) {
                break;
            }
            pix = *p++;
            mask = 0x80;
        }
    }
    while ((mp_hal_ticks_cpu() - startTime) < period) {
        ;                                           // Wait for last bit
    }
    mp_hal_quiet_timing_exit(irq_state);
}

#endif // MICROPY_ESP8266_NEOPIXEL
