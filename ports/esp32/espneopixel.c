// Original version from https://github.com/adafruit/Adafruit_NeoPixel
// Modifications by dpgeorge to support auto-CPU-frequency detection

// This is a mash-up of the Due show() code + insights from Michael Miller's
// ESP8266 work for the NeoPixelBus library: github.com/Makuna/NeoPixelBus
// Needs to be a separate .c file to enforce ICACHE_RAM_ATTR execution.

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "modesp.h"

/**
 * WS2812 timing
 * T0H: 0 code, high voltage time
 * T1H: 1 code, high voltage time
 * T0L: 0 code, low voltage time
 * T1L: 1 code, low voltage time
 */

// 400KHz ancient v1 neopixels
#define WS2811_TIMING 0
#define WS2811_T0H_US (0.5)
#define WS2811_T1H_US (1.2)
#define WS2811_T0L_US (2.0)
#define WS2811_T1L_US (1.3)

#define WS2812S_TIMING 1
#define WS2812S_T0H_US (0.35)
#define WS2812S_T1H_US (0.7)
#define WS2812S_T0L_US (0.8)
#define WS2812S_T1L_US (0.6)

#define WS2812B_TIMING 2
#define WS2812B_T0H_US (0.35)
#define WS2812B_T1H_US (0.9)
#define WS2812B_T0L_US (0.9)
#define WS2812B_T1L_US (0.35)

#define CPU_TICKS_HALFWAY (2147483648) // 2**31

__attribute__((always_inline)) static inline void wait_until_tick(uint32_t wait_until_tick) {
    // This function waits for the CPU clock cycle counting register to read the value specified
    //   in wait_until_tick, it is always inlined for speed.
    uint32_t current_ticks = mp_hal_ticks_cpu();
    // By dropping 6 clock cycles, we give enough time for loops to complete
    uint32_t before_tick = wait_until_tick - 6;
    if (before_tick < CPU_TICKS_HALFWAY) {
        // The tick we are waiting for is closer to the beginning
        while (current_ticks > CPU_TICKS_HALFWAY) {
            // Wait for the current tick to loop around (if necessary)
            current_ticks = mp_hal_ticks_cpu();
        }
        while (current_ticks < before_tick) {
            // Wait for the current tick to be after the tick we are waiting for
            current_ticks = mp_hal_ticks_cpu();
        }
    } else {
        // before_tick >= CPU_TICKS_HALFWAY
        // The tick we are waiting for is closer to the end
        while (current_ticks < CPU_TICKS_HALFWAY) {
            // Wait for the current tick to be in the final half
            current_ticks = mp_hal_ticks_cpu();
        }
        while ((current_ticks < before_tick) || (current_ticks < CPU_TICKS_HALFWAY)) {
            // Wait for the current tick to be after the tick we are waiting for
            //   including the loop around, if we miss it
            current_ticks = mp_hal_ticks_cpu();
        }
    }
}

uint32_t IRAM_ATTR esp_neopixel_write_timings(uint8_t pin, uint8_t *pixel_bytes, uint32_t numBytes, 
                                      uint32_t t0high, uint32_t t1high, uint32_t t0low, uint32_t t1low, uint32_t t_latch) {
    // This function will actually write to the neopixel with custom timings
    //   while it is normally called from the esp_neopixel_write function,
    //   you can call it directly from python code, particularly if you have
    //   the off-brand WS2812 style neopixels instead of the name brand
    //   Neopixels from Adafruit.
    // The timing params are in clock cycles
    uint8_t *pixel_ptr, *end, pixel_color_byte, color_bit_mask;
    uint32_t start_time, pin_mask;

    pin_mask         = 1 << pin;
    pixel_ptr        = pixel_bytes;
    end              = pixel_ptr + numBytes;
    pixel_color_byte = *pixel_ptr++;
    color_bit_mask   = 0x80;

    uint32_t irq_state = mp_hal_quiet_timing_enter(); // Disable interrupts
    start_time = mp_hal_ticks_cpu();
    while (true) {
        wait_until_tick(start_time);
        if (pixel_color_byte & color_bit_mask) {
            // Send a 1
            // Set high
            GPIO_REG_WRITE(GPIO_OUT_W1TS_REG, pin_mask);                 
            start_time += t1high;
            wait_until_tick(start_time);
            // Set low
            GPIO_REG_WRITE(GPIO_OUT_W1TC_REG, pin_mask);                 
            start_time += t1low;
        } else {
            // Send a 0
            // Set high
            GPIO_REG_WRITE(GPIO_OUT_W1TS_REG, pin_mask);
            start_time += t0high;
            wait_until_tick(start_time);
            // Set low
            GPIO_REG_WRITE(GPIO_OUT_W1TC_REG, pin_mask);
            start_time += t0low;
        }
        // Time is most flexible when the pin is low. For way too much info, see:
        //   https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
        // So we will do our loading logic here

        // Shift the mask to the next bit
        color_bit_mask >>= 1;
        if (color_bit_mask == 0) {
            // Pixel color byte is totally streamed out
            if(pixel_ptr >= end) {
                // All pixel bytes are sent
                break;
            } else {
                pixel_color_byte = *pixel_ptr++;
                color_bit_mask = 0x80;
            }
        }
    }
    mp_hal_quiet_timing_exit(irq_state); // Enable interrupts again
    wait_until_tick(start_time + t_latch); // Done streaming data
    return t0high;
}

uint32_t esp_neopixel_write(uint8_t pin, uint8_t *pixel_bytes, uint32_t numBytes, uint8_t timing) {
    uint32_t t1high, t1low, t0high, t0low, t_latch;
    uint32_t ticks_per_us = ets_get_cpu_frequency();

    if (timing == WS2811_TIMING) {
        // WS2811 400KHz design
        t0high = ticks_per_us * WS2811_T0H_US;
        t1high = ticks_per_us * WS2811_T1H_US;
        t0low = ticks_per_us * WS2811_T0L_US;
        t1low = ticks_per_us * WS2811_T1L_US;
    } else if (timing == WS2812S_TIMING) {
        // WS2812S
        t0high = ticks_per_us * WS2812S_T0H_US;
        t1high = ticks_per_us * WS2812S_T1H_US;
        t0low = ticks_per_us * WS2812S_T0L_US;
        t1low = ticks_per_us * WS2812S_T1L_US;
    } else { //if (timing == WS2812B_TIMING) {
        // WS2812B
        t0high = ticks_per_us * WS2812B_T0H_US;
        t1high = ticks_per_us * WS2812B_T1H_US;
        t0low = ticks_per_us * WS2812B_T0L_US;
        t1low = ticks_per_us * WS2812B_T1L_US;
    }
    // NOTE: If you add another timing version here
    //   (perhaps the APA106, WS2812D, WS2813, WS2815, PD9823, or the SK6812)
    //   then be sure to mention it in the getting started documentation:
    //     docs/esp32/quickref.rst
    // Also, you should add a new constant to the neopixel.py file
    // You can uncomment the commented if statement above.
    t_latch = ticks_per_us * 50;
    esp_neopixel_write_timings(pin, pixel_bytes, numBytes, t0high, t1high, t0low, t1low, t_latch);
    return t0high;
}
