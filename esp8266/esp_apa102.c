/*
 * Released under the The MIT License (MIT)
 *
 * @author Robert Foss <dev@robertfoss.se>
 */

#include "c_types.h"
#include "eagle_soc.h"
#include "user_interface.h"
#include "esp_apa102.h"


#define NOP asm volatile(" nop \n\t")


static inline void apa102_send_byte(uint32_t data_pin_mask, uint32_t clock_pin_mask, uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    if (byte & 0x80) {
      GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, data_pin_mask); // Set data pin high
    } else {
      GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, data_pin_mask); // Set data pin low
    }
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clock_pin_mask);  // Set clock pin high
    byte <<= 1;
    NOP;
    NOP;
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clock_pin_mask);  // Set clock pin low
    NOP;
    NOP;
  }
}


static void apa102_send_buffer(uint32_t data_pin_mask, uint32_t clock_pin_mask, uint32_t *buf, uint32_t nbr_frames) {
  // Send 32-bit Start Frame that's all 0x00
  apa102_send_byte(data_pin_mask, clock_pin_mask, 0x00);
  apa102_send_byte(data_pin_mask, clock_pin_mask, 0x00);
  apa102_send_byte(data_pin_mask, clock_pin_mask, 0x00);
  apa102_send_byte(data_pin_mask, clock_pin_mask, 0x00);

  // Send 32-bit LED Frames
  for (int i = 0; i < nbr_frames; i++) {
    uint8_t *byte = (uint8_t *) buf++;

    // Set the first 3 bits of that byte to 1.
    // This makes the lua interface easier to use since you
    // don't have to worry about creating invalid LED Frames.
    byte[0] |= 0xE0;
    apa102_send_byte(data_pin_mask, clock_pin_mask, byte[0]); // Brightness
    apa102_send_byte(data_pin_mask, clock_pin_mask, byte[3]); // R
    apa102_send_byte(data_pin_mask, clock_pin_mask, byte[2]); // G
    apa102_send_byte(data_pin_mask, clock_pin_mask, byte[1]); // B
  }

  // Send 32-bit End Frames
  uint32_t required_postamble_frames = (nbr_frames + 1) / 2;
  for (int i = 0; i < required_postamble_frames; i++) {
    apa102_send_byte(data_pin_mask, clock_pin_mask, 0xFF);
    apa102_send_byte(data_pin_mask, clock_pin_mask, 0xFF);
    apa102_send_byte(data_pin_mask, clock_pin_mask, 0xFF);
    apa102_send_byte(data_pin_mask, clock_pin_mask, 0xFF);
  }
}


// Byte quads in are interpreted as (brightness, B, G, R) values.
// Only the first 5 bits of the brightness value is actually used (0-31).
void /*ICACHE_RAM_ATTR*/ esp_apa102_write(uint8_t data_pin, uint8_t clock_pin, uint8_t *buf, uint32_t num_bytes) {
  uint32_t nbr_frames = num_bytes / 4;
  
  // Send the buffer
  uint32_t data_pin_mask  = 1 << data_pin;
  uint32_t clock_pin_mask = 1 << clock_pin;
  apa102_send_buffer(data_pin_mask, clock_pin_mask, (uint32_t *) buf, nbr_frames);
}

