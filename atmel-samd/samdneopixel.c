#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"

#include "mphalport.h"

#include "samdneopixel.h"

#ifdef MICROPY_HW_NEOPIXEL
static uint8_t status_neopixel_color[3];
#endif

extern void new_status_color(uint8_t r, uint8_t g, uint8_t b) {
    #ifdef MICROPY_HW_NEOPIXEL
        status_neopixel_color[0] = g;
        status_neopixel_color[1] = r;
        status_neopixel_color[2] = b;
        samd_neopixel_write(MICROPY_HW_NEOPIXEL, status_neopixel_color, 3, true);
    #endif
}
extern void temp_status_color(uint8_t r, uint8_t g, uint8_t b) {
    #ifdef MICROPY_HW_NEOPIXEL
        uint8_t colors[3] = {g, r, b};
        samd_neopixel_write(MICROPY_HW_NEOPIXEL, colors, 3, true);
    #endif
}
extern void clear_temp_status() {
    #ifdef MICROPY_HW_NEOPIXEL
        samd_neopixel_write(MICROPY_HW_NEOPIXEL, status_neopixel_color, 3, true);
    #endif
}

void samd_neopixel_write(uint32_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz) {
  // This is adapted directly from the Adafruit NeoPixel library SAMD21G18A code:
  // https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
  uint8_t  *ptr, *end, p, bitMask;
  uint32_t  pinMask;
  PortGroup* port;

  // Turn off interrupts of any kind during timing-sensitive code.
  mp_hal_disable_all_interrupts();

  port    =  port_get_group_from_gpio_pin(pin);
  pinMask =  (1UL << (pin % 32));  // From port_pin_set_output_level ASF code.
  ptr     =  pixels;
  end     =  ptr + numBytes;
  p       = *ptr++;
  bitMask =  0x80;

  volatile uint32_t *set = &(port->OUTSET.reg),
                    *clr = &(port->OUTCLR.reg);

  if(is800KHz) {
    for(;;) {
      *set = pinMask;
      asm("nop; nop; nop; nop; nop; nop; nop; nop;");
      if(p & bitMask) {
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop;");
        *clr = pinMask;
      } else {
        *clr = pinMask;
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop;");
      }
      if(bitMask >>= 1) {
        asm("nop; nop; nop; nop; nop; nop; nop; nop; nop;");
      } else {
        if(ptr >= end) break;
        p       = *ptr++;
        bitMask = 0x80;
      }
    }
  } else { // 400 KHz bitstream
    for(;;) {
      *set = pinMask;
      asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
      if(p & bitMask) {
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop;");
        *clr = pinMask;
      } else {
        *clr = pinMask;
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop;");
      }
      asm("nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop; nop; nop; nop; nop;"
          "nop; nop; nop; nop; nop; nop; nop; nop;");
      if(bitMask >>= 1) {
        asm("nop; nop; nop; nop; nop; nop; nop;");
      } else {
        if(ptr >= end) break;
        p       = *ptr++;
        bitMask = 0x80;
      }
    }
  }

  // Turn on interrupts after timing-sensitive code.
  mp_hal_enable_all_interrupts();

  // 50us delay to let pixels latch to the data that was just sent.
  // This could be optimized to only occur before pixel writes when necessary,
  // like in the Arduino library.
  delay_us(50);
}
