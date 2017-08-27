/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "samd21_pins.h"
#include "tick.h"

#include "asf/sam0/drivers/sercom/sercom_interrupt.h"

#undef ENABLE

busio_uart_obj_t *_uart_instances[SERCOM_INST_NUM];

static void _sercom_default_handler(
        const uint8_t instance)
{
    Assert(false);
}

static void _busio_uart_interrupt_handler(uint8_t instance)
{
    /* Temporary variables */
    uint16_t interrupt_status;
    uint8_t error_code;

    /* Get device instance from the look-up table */
    struct usart_module *module
        = (struct usart_module *)_sercom_instances[instance];

    busio_uart_obj_t *self = _uart_instances[instance];

    /* Pointer to the hardware module instance */
    SercomUsart *const usart_hw = &(module->hw->USART);

    /* Wait for the synchronization to complete */
    _usart_wait_for_sync(module);

    /* Read and mask interrupt flag register */
    interrupt_status = usart_hw->INTFLAG.reg;
    interrupt_status &= usart_hw->INTENSET.reg;

    /* Check if the Receive Complete interrupt has occurred, and that
     * there's more data to receive */
    if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {
        /* Read out the status code and mask away all but the 4 LSBs*/
        error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
        /* CTS status should not be considered as an error */
        if(error_code & SERCOM_USART_STATUS_CTS) {
            error_code &= ~SERCOM_USART_STATUS_CTS;
        }
        /* Check if an error has occurred during the receiving */
        if (error_code) {
            /* Check which error occurred */
            if (error_code & SERCOM_USART_STATUS_FERR) {
                /* Store the error code and clear flag by writing 1 to it */
                usart_hw->STATUS.reg = SERCOM_USART_STATUS_FERR;
            } else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
                /* Store the error code and clear flag by writing 1 to it */
                usart_hw->STATUS.reg = SERCOM_USART_STATUS_BUFOVF;
            } else if (error_code & SERCOM_USART_STATUS_PERR) {
                /* Store the error code and clear flag by writing 1 to it */
                usart_hw->STATUS.reg = SERCOM_USART_STATUS_PERR;
            }
            self->rx_error = true;
        } else {
            /* Read current packet from DATA register,
             * increment buffer pointer and decrement buffer length */
            uint16_t received_data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);

            common_hal_mcu_disable_interrupts();
            /* Read value will be at least 8-bits long */
            uint32_t buffer_end = (self->buffer_start + self->buffer_size) % self->buffer_length;
            self->buffer[buffer_end] = received_data;
            self->buffer_size++;

            if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
                buffer_end = (self->buffer_start + self->buffer_size) % self->buffer_length;
                /* 9-bit data, write next received byte to the buffer */
                self->buffer[buffer_end] = (received_data >> 8);
                self->buffer_size++;
            }

            if (self->buffer_size > self->buffer_length) {
                self->buffer_start++;
                if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
                    self->buffer_start++;
                }
                self->buffer_size = self->buffer_length;
            }
            common_hal_mcu_enable_interrupts();
        }
    }
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
        const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx, uint32_t baudrate,
        uint8_t bits, uart_parity_t parity, uint8_t stop, uint32_t timeout,
        uint8_t receiver_buffer_size) {
    Sercom* sercom = NULL;
    uint32_t rx_pinmux = PINMUX_UNUSED;
    uint8_t rx_pad = 5; // Unset pad
    uint32_t tx_pinmux = PINMUX_UNUSED;
    uint8_t tx_pad = 5; // Unset pad
    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        Sercom* potential_sercom = NULL;
        if (tx != NULL) {
            potential_sercom = tx->sercom[i].sercom;
            if (potential_sercom == NULL ||
                potential_sercom->I2CM.CTRLA.bit.ENABLE != 0 ||
                !(tx->sercom[i].pad == 0 ||
                  tx->sercom[i].pad == 2)) {
                continue;
            }
            tx_pinmux = PINMUX(tx->pin, (i == 0) ? MUX_C : MUX_D);
            tx_pad = tx->sercom[i].pad;
            if (rx == NULL) {
                sercom = potential_sercom;
                break;
            }
        }
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            if (((tx == NULL && rx->sercom[j].sercom->I2CM.CTRLA.bit.ENABLE == 0) ||
                 potential_sercom == rx->sercom[j].sercom) &&
                rx->sercom[j].pad != tx_pad) {
                rx_pinmux = PINMUX(rx->pin, (j == 0) ? MUX_C : MUX_D);
                rx_pad = rx->sercom[j].pad;
                sercom = rx->sercom[j].sercom;
                break;
            }
        }
        if (sercom != NULL) {
            break;
        }
    }
    if (sercom == NULL) {
        mp_raise_ValueError("Invalid pins");
    }
    if (tx == NULL) {
        tx_pad = 0;
        if (rx_pad == 0) {
            tx_pad = 2;
        }
    }
    if (rx == NULL) {
        rx_pad = (tx_pad + 1) % 4;
    }
    struct usart_config config_usart;
    usart_get_config_defaults(&config_usart);
    config_usart.mux_setting = (SERCOM_USART_CTRLA_RXPO(rx_pad) | SERCOM_USART_CTRLA_TXPO(tx_pad / 2));

    if (parity == PARITY_ODD) {
        config_usart.parity = USART_PARITY_ODD;
    } else if (parity == PARITY_EVEN) {
        config_usart.parity = USART_PARITY_EVEN;
    }
    config_usart.stopbits = stop - 1;
    config_usart.character_size = bits % 8;
    config_usart.baudrate = baudrate;

    // Map pad to pinmux through a short array.
    uint32_t *pinmuxes[4] = {&config_usart.pinmux_pad0,
                             &config_usart.pinmux_pad1,
                             &config_usart.pinmux_pad2,
                             &config_usart.pinmux_pad3};
    // Pin muxes have a default pin, set them to unused so that no other pins are changed.
    for (int i = 0; i < 4; i++) {
        *pinmuxes[i] = PINMUX_UNUSED;
    }

    self->rx_pin = NO_PIN;
    config_usart.receiver_enable = rx != NULL;
    if (rx != NULL) {
        *pinmuxes[rx_pad] = rx_pinmux;
        self->rx_pin = rx->pin;
        claim_pin(rx);
    }

    self->tx_pin = NO_PIN;
    config_usart.transmitter_enable = tx != NULL;
    if (tx != NULL) {
        *pinmuxes[tx_pad] = tx_pinmux;
        self->tx_pin = tx->pin;
        claim_pin(tx);
    }

    self->timeout_ms = timeout;

    self->buffer_length = receiver_buffer_size;
    self->buffer_length *= (bits + 7) / 8;
    self->buffer = (uint8_t *) gc_alloc(self->buffer_length * sizeof(uint8_t), false);
    if (self->buffer == NULL) {
        common_hal_busio_uart_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "Failed to allocate RX buffer");
    }

    if (usart_init(&self->uart_instance, sercom, &config_usart) != STATUS_OK) {
        common_hal_busio_uart_deinit(self);
        mp_raise_OSError(MP_EIO);
    }

    // We use our own interrupt handler because we want a circular buffer
    // instead of the jobs that ASF provides.
    uint8_t instance_index = _sercom_get_sercom_inst_index(self->uart_instance.hw);
    _sercom_set_handler(instance_index, _busio_uart_interrupt_handler);
    _sercom_instances[instance_index] = &self->uart_instance;
    _uart_instances[instance_index] = self;

    /* Enable Global interrupt for module */
    system_interrupt_enable(_sercom_get_interrupt_vector(self->uart_instance.hw));

    usart_enable(&self->uart_instance);
    self->uart_instance.hw->USART.INTENSET.bit.RXC = true;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    self->uart_instance.hw->USART.INTENCLR.bit.RXC = true;

    uint8_t instance_index = _sercom_get_sercom_inst_index(self->uart_instance.hw);
    _sercom_set_handler(instance_index, &_sercom_default_handler);
    _sercom_instances[instance_index] = NULL;
    _uart_instances[instance_index] = NULL;

    system_interrupt_disable(_sercom_get_interrupt_vector(self->uart_instance.hw));

    usart_disable(&self->uart_instance);
    reset_pin(self->rx_pin);
    reset_pin(self->tx_pin);
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    size_t total_read = 0;
    uint64_t start_ticks = ticks_ms;
    while (total_read < len && ticks_ms - start_ticks < self->timeout_ms) {
        if (self->buffer_size > 0) {
            common_hal_mcu_disable_interrupts();
            data[total_read] = self->buffer[self->buffer_start];
            if (self->uart_instance.character_size == USART_CHARACTER_SIZE_9BIT) {
                data[total_read + 1] = self->buffer[self->buffer_start + 1];
                self->buffer_start += 2;
                self->buffer_size -= 2;
            } else {
                self->buffer_start++;
                self->buffer_size--;
            }
            self->buffer_start  = self->buffer_start % self->buffer_length;
            common_hal_mcu_enable_interrupts();
            // Reset the timeout every character read.
            total_read++;
            start_ticks = ticks_ms;
        }
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
    }
    if (total_read == 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return total_read;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    /* Check that the transmitter is enabled */
    if (!(self->uart_instance.transmitter_enabled)) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    /* Get a pointer to the hardware module instance */
    SercomUsart *const usart_hw = &(self->uart_instance.hw->USART);

    /* Wait until synchronization is complete */
    _usart_wait_for_sync(&self->uart_instance);

    uint16_t tx_pos = 0;

    bool ok = true;
    uint64_t start_ticks = 0;
    /* Blocks while buffer is being transferred */
    while (len--) {
        /* Wait for the USART to be ready for new data and abort
        * operation if it doesn't get ready within the timeout*/
        ok = false;
        start_ticks = ticks_ms;
        while (ticks_ms - start_ticks < self->timeout_ms) {
            if (usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE) {
                ok = true;
                break;
            }
            #ifdef MICROPY_VM_HOOK_LOOP
                MICROPY_VM_HOOK_LOOP
            #endif
        }

        if (!ok) {
            break;
        }

        /* Data to send is at least 8 bits long */
        uint16_t data_to_send = data[tx_pos++];

        /* Check if the character size exceeds 8 bit */
        if (self->uart_instance.character_size == USART_CHARACTER_SIZE_9BIT) {
            data_to_send |= (data[tx_pos++] << 8);
        }

        /* Send the data through the USART module */

        enum status_code status = usart_write_wait(&self->uart_instance, data_to_send);
        if (status != STATUS_OK) {
            ok = false;
        }
    }

    /* Wait until Transmit is complete or timeout */
    if (ok) {
        ok = false;
        start_ticks = ticks_ms;
        while (ticks_ms - start_ticks < self->timeout_ms) {
            if (usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {
                ok = true;
                break;
            }
            #ifdef MICROPY_VM_HOOK_LOOP
                MICROPY_VM_HOOK_LOOP
            #endif
        }
    }

    if (!ok && tx_pos == 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return tx_pos;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    if (self->uart_instance.character_size == USART_CHARACTER_SIZE_9BIT) {
        return self->buffer_size / 2;
    }
    return self->buffer_size;
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (!(self->uart_instance.transmitter_enabled)) {
        return false;
    }
    return self->uart_instance.hw->USART.INTFLAG.bit.DRE;
}
