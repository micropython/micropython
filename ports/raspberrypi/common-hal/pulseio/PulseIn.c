/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dave Putz for Adafruit Industries
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
<<<<<<< HEAD
=======
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d

#include <stdint.h>

#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"
#include "bindings/rp2pio/StateMachine.h"
#include "common-hal/pulseio/PulseIn.h"

pulseio_pulsein_obj_t* save_self;

#define NO_PIN 0xff
volatile bool last_level;
volatile uint16_t level_count = 0;
volatile uint16_t result = 0;
volatile uint16_t buf_index = 0;

uint16_t pulsein_program[] = {
<<<<<<< HEAD
    0x4001, //  1: in     pins, 1
=======
    0xe03f, //  0: set    x, 31
    0x4001, //  1: in     pins, 1
    0x0041, //  2: jmp    x--, 2
    0x8060, //  3: push   iffull block
    0xc020, //  4: irq    wait 0
    0x0000, //  5: jmp    1
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
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
<<<<<<< HEAD
        1000000,
=======
        1000000 * 3,
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
        NULL, 0,
        NULL, 0,
        pin, 1,
        0,0,
        NULL, 0,
        NULL, 0,
        1, 0,
        1 << self->pin, false, true,
        false, 8, false, // TX, unused
        false,
        true, 32, true, // RX auto-push every 32 bits
        false); // claim pins
    pio_sm_set_enabled(state_machine.pio,state_machine.state_machine, false);
    self->state_machine.pio = state_machine.pio;
    self->state_machine.state_machine = state_machine.state_machine;
    self->state_machine.sm_config = state_machine.sm_config;
    self->state_machine.offset = state_machine.offset;
<<<<<<< HEAD
=======
    if ( self->state_machine.pio == pio0 ) {
       self->pio_interrupt = PIO0_IRQ_0;
    } else {
       self->pio_interrupt = PIO1_IRQ_0;
    }
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
    pio_sm_clear_fifos(self->state_machine.pio,self->state_machine.state_machine);
    last_level = self->idle_state;
    level_count = 0;
    result = 0;
    buf_index = 0;

    pio_sm_set_in_pins(state_machine.pio,state_machine.state_machine,pin->number);
<<<<<<< HEAD
    common_hal_rp2pio_statemachine_set_interrupt_handler(&state_machine,&common_hal_pulseio_pulsein_interrupt,NULL,PIO_IRQ0_INTE_SM0_RXNEMPTY_BITS);
=======
    irq_set_exclusive_handler(self->pio_interrupt, common_hal_pulseio_pulsein_interrupt);
    hw_clear_bits(&state_machine.pio->inte0, 1u << state_machine.state_machine);
    hw_set_bits(&state_machine.pio->inte0, 1u << (state_machine.state_machine+8));
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d

    // exec a set pindirs to 0 for input
    pio_sm_exec(state_machine.pio,state_machine.state_machine,0xe080);
    //exec the appropriate wait for pin
    if (self->idle_state == true ) {
       pio_sm_exec(self->state_machine.pio,self->state_machine.state_machine,0x2020);
    } else {
       pio_sm_exec(self->state_machine.pio,self->state_machine.state_machine,0x20a0);
    }
    pio_sm_set_enabled(state_machine.pio, state_machine.state_machine, true);
<<<<<<< HEAD
=======
    irq_set_enabled(self->pio_interrupt, true);
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t* self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
<<<<<<< HEAD
=======
    irq_set_enabled(self->pio_interrupt, false);
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
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
    uint32_t rxfifo = 0;

    rxfifo = pio_sm_get_blocking(self->state_machine.pio, self->state_machine.state_machine);
    // translate from fifo to buffer
    for (uint i = 0; i < 32; i++) {
        bool level = (rxfifo & (1 << i)) >> i;
        if (level == last_level ) {
            level_count ++;
        } else {
           result = level_count;
           last_level = level;
           level_count = 1;
           // ignore pulses that are too long and too short
           if (result < 4000 && result > 10) {
               self->buffer[buf_index] = result;
               buf_index++;
               self->len++;
             }
           }
       }
<<<<<<< HEAD
=======
// clear interrupt
    irq_clear(self->pio_interrupt);
    hw_clear_bits(&self->state_machine.pio->inte0, 1u << self->state_machine.state_machine);
    self->state_machine.pio->irq = 1u << self->state_machine.state_machine;
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
// check for a pulse thats too long (4000 us) or maxlen reached,  and reset
    if (( level_count > 4000 ) || (buf_index >= self->maxlen)) {
       pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
       pio_sm_init(self->state_machine.pio, self->state_machine.state_machine, self->state_machine.offset, &self->state_machine.sm_config);
       pio_sm_restart(self->state_machine.pio,self->state_machine.state_machine);
       pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, true);
<<<<<<< HEAD
=======
       irq_set_enabled(self->pio_interrupt, true);
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
    }
}
void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self,
        uint16_t trigger_duration) {
    // exec a wait for the selected pin to change state
    if (self->idle_state == true ) {
       pio_sm_exec(self->state_machine.pio,self->state_machine.state_machine,0x2020);
    } else {
       pio_sm_exec(self->state_machine.pio,self->state_machine.state_machine,0x20a0);
    }
    // Send the trigger pulse.
    if (trigger_duration > 0) {
        gpio_set_function(self->pin ,GPIO_FUNC_SIO);
        gpio_set_dir(self->pin,true);
        gpio_put(self->pin, !self->idle_state);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        gpio_set_function(self->pin ,GPIO_FUNC_PIO0);
<<<<<<< HEAD
        common_hal_mcu_delay_us(225);
    }

    // Reconfigure the pin for PIO
=======
    }

    // Reconfigure the pin for PIO
    common_hal_mcu_delay_us(200);
>>>>>>> a3c3e8a0fa5e06910747f1a95a12b899562a618d
    gpio_set_function(self->pin, GPIO_FUNC_PIO0);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, true);
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
    // if we are empty reset buffer pointer and counters
    if (self->len == 0 ) {
         self->start = 0;
         buf_index = 0;
         level_count = 0;
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
