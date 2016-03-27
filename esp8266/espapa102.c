// Original version from https://github.com/adafruit/Adafruit_NeoPixel
// Modifications by dpgeorge to support auto-CPU-frequency detection

// This is a mash-up of the Due show() code + insights from Michael Miller's
// ESP8266 work for the NeoPixelBus library: github.com/Makuna/NeoPixelBus
// Needs to be a separate .c file to enforce ICACHE_RAM_ATTR execution.

#include <stdio.h>
#include "c_types.h"
#include "eagle_soc.h"
#include "user_interface.h"
#include "espapa102.h"

void esp_apa102_write(uint8_t clockPin, uint8_t dataPin, uint8_t *pixels, uint32_t numBytes) {
    uint32_t clockPinMask, dataPinMask;

    clockPinMask = 1 << clockPin;
    dataPinMask  = 1 << dataPin;

    // start the frame
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, dataPinMask);

    for (uint32_t i = 0; i < 32; i++) {
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clockPinMask);
        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clockPinMask);
    }

    // write pixels
    for (uint32_t i = 0; i < numBytes / 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            uint8_t byte = pixels[i * 4 + (3 - j)] | (j == 0 ? 0xE0 : 0x00);

            for (uint32_t k = 0; k < 8; k++) {
                uint32_t dataHigh = (byte >> (7 - k)) & 0x01;

                if (dataHigh) {
                    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, dataPinMask);
                }
                GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clockPinMask);

                GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clockPinMask);
                if (dataHigh) {
                    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, dataPinMask);
                }
            }
        }
    }

    // end the frame
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, dataPinMask);

    // we need to write some more clock cycles, because each led
    // delays the data by one edge after inverting the clock
    for (uint32_t i = 0; i < numBytes / 4 - 1; i++) {
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clockPinMask);
        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clockPinMask);
    }

    for (uint32_t i = 0; i < 32; i++) {
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clockPinMask);
        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clockPinMask);
    }
}
