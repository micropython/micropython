// Original version from https://github.com/adafruit/Adafruit_NeoPixel
// Modifications by dpgeorge to support auto-CPU-frequency detection

// This is a mash-up of the Due show() code + insights from Michael Miller's
// ESP8266 work for the NeoPixelBus library: github.com/Makuna/NeoPixelBus
// Needs to be a separate .c file to enforce ICACHE_RAM_ATTR execution.

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "modesp.h"

void IRAM_ATTR esp_neopixel_write(uint8_t pin, uint8_t *pixels, uint32_t numBytes, uint8_t timing) {
    uint8_t *p, *end, pix, mask;
    uint32_t t, time0, time1, period, c, startTime, pinMask;

    pinMask   = 1 << pin;
    p         =  pixels;
    end       =  p + numBytes;
    pix       = *p++;
    mask      = 0x80;
    startTime = 0;

    uint32_t fcpu = ets_get_cpu_frequency() * 1000000;

    if (timing == 1) {
        // 800 KHz
        time0 = (fcpu * 0.35) / 1000000; // 0.35us
        time1 = (fcpu * 0.8) / 1000000; // 0.8us
        period = (fcpu * 1.25) / 1000000; // 1.25us per bit
    } else {
        // 400 KHz
        time0 = (fcpu * 0.5) / 1000000; // 0.35us
        time1 = (fcpu * 1.2) / 1000000; // 0.8us
        period = (fcpu * 2.5) / 1000000; // 1.25us per bit
    }

    uint32_t irq_state = mp_hal_quiet_timing_enter();
    for (t = time0;; t = time0) {
        if (pix & mask) t = time1;                                  // Bit high duration
        while (((c = mp_hal_ticks_cpu()) - startTime) < period);    // Wait for bit start
        GPIO_REG_WRITE(GPIO_OUT_W1TS_REG, pinMask);                 // Set high
        startTime = c;                                              // Save start time
        while (((c = mp_hal_ticks_cpu()) - startTime) < t);         // Wait high duration
        GPIO_REG_WRITE(GPIO_OUT_W1TC_REG, pinMask);                 // Set low
        if (!(mask >>= 1)) {                                        // Next bit/byte
            if(p >= end) break;
            pix  = *p++;
            mask = 0x80;
        }
    }
    while ((mp_hal_ticks_cpu() - startTime) < period); // Wait for last bit
    mp_hal_quiet_timing_exit(irq_state);
}
