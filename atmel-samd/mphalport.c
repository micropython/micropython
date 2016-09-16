#include <string.h>

#include "asf/common/services/usb/class/cdc/device/udi_cdc.h"
#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "py/mphal.h"
#include "py/mpstate.h"

#include "mpconfigboard.h"
#include "mphalport.h"

// Store received characters on our own so that we can filter control characters
// and act immediately on CTRL-C for example.
// This is adapted from asf/thirdparty/wireless/addons/sio2host

// Receive buffer
static uint8_t usb_rx_buf[USB_RX_BUF_SIZE];

// Receive buffer head
static uint8_t usb_rx_buf_head;

// Receive buffer tail
static uint8_t usb_rx_buf_tail;

// Number of bytes in receive buffer
static volatile uint8_t usb_rx_count;

static volatile bool mp_cdc_enabled = false;

void mp_keyboard_interrupt(void);
int interrupt_char;

extern struct usart_module usart_instance;

bool mp_cdc_enable(uint8_t port)
{
	mp_cdc_enabled = true;
	return true;
}

void mp_cdc_disable(uint8_t port)
{
	mp_cdc_enabled = false;
}

void usb_rx_notify(void)
{
    if (mp_cdc_enabled) {
        while (udi_cdc_is_rx_ready()) {
            uint8_t c;
            c = udi_cdc_getc();

            if (c == interrupt_char) {
                mp_keyboard_interrupt();
                // Don't put the interrupt into the buffer, just continue.
                continue;
            }

            // Introducing critical section to avoid buffer corruption.
            cpu_irq_disable();

            // The count of characters present in receive buffer is
            // incremented.
            usb_rx_count++;
            usb_rx_buf[usb_rx_buf_tail] = c;

            if ((USB_RX_BUF_SIZE - 1) == usb_rx_buf_tail) {
                // Reached the end of buffer, revert back to beginning of
                // buffer.
                usb_rx_buf_tail = 0x00;
            } else {
                usb_rx_buf_tail++;
            }

            cpu_irq_enable();
        }
    }
}

int receive_usb() {
    if (0 == usb_rx_count) {
        return 0;
    }

    if (USB_RX_BUF_SIZE <= usb_rx_count) {
        // Bytes between head and tail are overwritten by new data. The
        // oldest data in buffer is the one to which the tail is pointing. So
        // reading operation should start from the tail.
        usb_rx_buf_head = usb_rx_buf_tail;

        // This is a buffer overflow case.But still only the number of bytes
        // equivalent to full buffer size are useful.
        usb_rx_count = USB_RX_BUF_SIZE;
    }

    // Copy from head.
    int data = usb_rx_buf[usb_rx_buf_head];
    usb_rx_buf_head++;
    usb_rx_count--;
    if ((USB_RX_BUF_SIZE) == usb_rx_buf_head) {
      usb_rx_buf_head = 0;
    }
    return data;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #ifdef USB_REPL
        if (mp_cdc_enabled && usb_rx_count > 0) {
            #ifdef MICROPY_HW_LED_RX
            port_pin_toggle_output_level(MICROPY_HW_LED_RX);
            #endif
            return receive_usb();
        }
        #endif
        #ifdef UART_REPL
        uint16_t temp;
        if (usart_read_wait(&usart_instance, &temp) == STATUS_OK) {
            #ifdef MICROPY_HW_LED_RX
            port_pin_toggle_output_level(MICROPY_HW_LED_RX);
            #endif
            return temp;
        }
        #endif
        // TODO(tannewt): Figure out how we can sleep while waiting for input and
        // add it here. The current UART implementation doesn't cause a wake.
        //__WFI();
    }
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    #ifdef MICROPY_HW_LED_TX
    port_pin_toggle_output_level(MICROPY_HW_LED_TX);
    #endif

    #ifdef UART_REPL
    usart_write_buffer_wait(&usart_instance, (uint8_t*) str, len);
    #endif

    #ifdef USB_REPL
    if (mp_cdc_enabled && udi_cdc_is_tx_ready()) {
        udi_cdc_write_buf(str, len);
    }
    #endif
}

void mp_hal_set_interrupt_char(int c) {
    if (c != -1) {
        mp_obj_exception_clear_traceback(MP_STATE_PORT(mp_kbd_exception));
    }
    extern int interrupt_char;
    interrupt_char = c;
}

void mp_hal_delay_ms(mp_uint_t delay) {
  delay_ms(delay);
}

void mp_hal_delay_us(mp_uint_t delay) {
  delay_us(delay);
}
