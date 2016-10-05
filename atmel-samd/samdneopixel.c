#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"

#include "samdneopixel.h"

void samd_neopixel_write(uint32_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz) {
  // This is adapted directly from the Adafruit NeoPixel library SAMD21G18A code:
  // https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
  uint8_t  *ptr, *end, p, bitMask;
  uint32_t  pinMask;
  PortGroup* port;

  // Turn off interrupts of any kind during timing-sensitive code.
  irqflags_t flags = cpu_irq_save();

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
  cpu_irq_restore(flags);

  // 50ms delay to let pixels latch to the data that was just sent.
  // This could be optimized to only occur before pixel writes when necessary,
  // like in the Arduino library.
  delay_ms(50);
}
