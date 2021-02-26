/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2021 Scott Shawcroft for Adafruit Industries
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

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"

#include <stdint.h>

#include "background.h"
#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"
#include "bindings/rp2pio/StateMachine.h"
#include "common-hal/pulseio/PulseIn.h"

pulseio_pulsein_obj_t* save_self;

#define NO_PIN 0xff

const uint16_t pulsein_program[] = {
//   wait   0 pin, 0   ; Wait for first low to start
    0x2020,
//   irq wait 0        ; set IRQ 0 and wait
    0xc020,
//   .bitloop
//   in    pins, 1 [1] ; sample every 3 cycles (2 instructions, 1 delay)
    0x4101,
//   jmp bitloop
    0x0002,
};

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t* self,
        const mcu_pin_obj_t* pin, uint16_t maxlen, bool idle_state) {

    self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, translate("Failed to allocate RX buffer of %d bytes"), maxlen * sizeof(uint16_t));
    }
    self->pin = pin->number;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    save_self = self;

    // Set everything up.
    rp2pio_statemachine_obj_t state_machine;

    bool ok = rp2pio_statemachine_construct(&state_machine,
        pulsein_program, sizeof(pulsein_program) / sizeof(pulsein_program[0]),
        125000 * 4,
        NULL, 0,
        NULL, 0,
        pin, 1,
        NULL, 0,
        NULL, 0,
        1 << self->pin, false, true,
        false, 8, false, // TX, unused
        true, 32, true, // RX auto-push every 32 bits
        false); // claim pins
    self->state_machine.pio = state_machine.pio;
    self->state_machine.state_machine = state_machine.state_machine;
    self->state_machine.sm_config = state_machine.sm_config;
    self->state_machine.offset = state_machine.offset;
    if ( self->state_machine.pio == pio0 ) {
       self->pio_interrupt = PIO0_IRQ_0;
    } else {
       self->pio_interrupt = PIO1_IRQ_0;
    }

    pio_sm_set_in_pins(self->state_machine.pio,self->state_machine.state_machine,pin->number);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
    irq_set_exclusive_handler(self->pio_interrupt, common_hal_pulseio_pulsein_interrupt);
    irq_set_enabled(self->pio_interrupt, true);
    hw_clear_bits(&self->state_machine.pio->inte0, 1u << self->state_machine.state_machine);
    hw_set_bits(&self->state_machine.pio->inte0, 1u << (self->state_machine.state_machine+8));
    // exec a set pindirs to 0 for input
    pio_sm_exec(self->state_machine.pio,self->state_machine.state_machine,0xe080);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, true);

}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t* self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
    irq_set_enabled(self->pio_interrupt, false);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
    pio_sm_unclaim (self->state_machine.pio, self->state_machine.state_machine);
    m_free(self->buffer);
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t* self) {
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
}

void common_hal_pulseio_pulsein_interrupt() {

    pulseio_pulsein_obj_t* self = save_self;
// clear interrupt
    hw_clear_bits(&self->state_machine.pio->inte0, 1u << self->state_machine.state_machine);
    self->state_machine.pio->irq = 1u << self->state_machine.state_machine;
    irq_clear(self->pio_interrupt);
    pio_sm_clear_fifos(self->state_machine.pio,self->state_machine.state_machine);
    bool last_level = true;
    uint level_count = 0;
    uint16_t result = 0;
    uint16_t buf_index = 0;
    while ( buf_index < self->maxlen ) {
       uint32_t rxfifo = 0;
       rxfifo = pio_sm_get_blocking(self->state_machine.pio, self->state_machine.state_machine);
       // translate from fifo to buffer
       for (uint i = 0; i < 32; i++) {
           bool level = (rxfifo & (1 << i)) >> i;
           if (level == last_level ) {
             level_count ++;
           } else {
             result = level_count * 6;
             last_level = level;
             level_count = 1;
           // ignore pulses that are too long and too short
             if (result < 10000 && result > 10) {
               self->buffer[buf_index] = result;
               buf_index++;
               self->len++;
             }
           }
       }
       // check for a pulse thats too long (20ms)
       if ( level_count > 3000 ) {
            break;
       }
    }
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
    pio_sm_init(self->state_machine.pio, self->state_machine.state_machine, self->state_machine.offset, &self->state_machine.sm_config);
    pio_sm_restart(self->state_machine.pio,self->state_machine.state_machine);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, true);
    irq_set_enabled(self->pio_interrupt, true);
}
void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self,
        uint16_t trigger_duration) {
    // Send the trigger pulse.
    if (trigger_duration > 0) {
        gpio_set_function(self->pin ,GPIO_FUNC_SIO);
        gpio_set_dir(self->pin,true);
        gpio_put(self->pin, !self->idle_state);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        gpio_set_function(self->pin ,GPIO_FUNC_PIO0);
    }

    // Reconfigure the pin for PIO
    common_hal_mcu_delay_us(100);
    gpio_set_function(self->pin, GPIO_FUNC_PIO0);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, true);
    // exec a wait for the selected pin to go high
    pio_sm_exec(self->state_machine.pio,self->state_machine.state_machine,0x20a0);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t* self) {
    self->start = 0;
    self->len = 0;
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t* self) {
    if (self->len == 0) {
        mp_raise_IndexError_varg(translate("pop from empty %q"), MP_QSTR_PulseIn);
    }
    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;
    if (self->len == 0 ) {
    // reset buffer pointer
         self->start = 0;
    }
    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t* self) {
    return self->maxlen;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t* self) {
    return self->len;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t* self) {
    return true;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t* self,
        int16_t index) {
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        mp_raise_IndexError_varg(translate("%q index out of range"), MP_QSTR_PulseIn);
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    return value;
}
