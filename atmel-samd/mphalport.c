#include <string.h>

#include "autoreset.h"
#include "compiler.h"
#include "asf/common/services/sleepmgr/sleepmgr.h"
#include "asf/common/services/usb/class/cdc/device/udi_cdc.h"
#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/smallint.h"

#include "mpconfigboard.h"
#include "mphalport.h"

// Store received characters on our own so that we can filter control characters
// and act immediately on CTRL-C for example.
// This is adapted from asf/thirdparty/wireless/addons/sio2host

// Receive buffer
static uint8_t usb_rx_buf[USB_RX_BUF_SIZE];

// Receive buffer head
static volatile uint8_t usb_rx_buf_head;

// Receive buffer tail
static volatile uint8_t usb_rx_buf_tail;

// Number of bytes in receive buffer
static volatile uint8_t usb_rx_count;

static volatile bool mp_cdc_enabled = false;

void mp_keyboard_interrupt(void);
int interrupt_char;

extern struct usart_module usart_instance;


static volatile bool mp_msc_enabled = false;
bool mp_msc_enable()
{
	mp_msc_enabled = true;
	return true;
}

void mp_msc_disable()
{
	mp_msc_enabled = false;
}

bool mp_cdc_enable(uint8_t port)
{
	mp_cdc_enabled = false;
	return true;
}

void mp_cdc_disable(uint8_t port)
{
	mp_cdc_enabled = false;
}

void usb_dtr_notify(uint8_t port, bool set) {
	mp_cdc_enabled = set;
}

void usb_rts_notify(uint8_t port, bool set) {
    return;
}

void inject_character(char c) {
    // Introduce a critical section to avoid buffer corruption. We use
    // cpu_irq_save instead of cpu_irq_disable because we don't know the
    // current state of IRQs. They may have been turned off already and
    // we don't want to accidentally turn them back on.
    irqflags_t flags = cpu_irq_save();
    // If our buffer is full, then don't get another character otherwise
    // we'll lose a previous character.
    if (usb_rx_count >= USB_RX_BUF_SIZE) {
        cpu_irq_restore(flags);
        return;
    }

    uint8_t current_tail = usb_rx_buf_tail;
    // Pretend we've received a character so that any nested calls to
    // this function have to consider the spot we've reserved.
    if ((USB_RX_BUF_SIZE - 1) == usb_rx_buf_tail) {
        // Reached the end of buffer, revert back to beginning of
        // buffer.
        usb_rx_buf_tail = 0x00;
    } else {
        usb_rx_buf_tail++;
    }
    // The count of characters present in receive buffer is
    // incremented.
    usb_rx_count++;

    // We put the next character where we expected regardless of whether
    // the next character was already loaded in the buffer.
    usb_rx_buf[current_tail] = c;

    cpu_irq_restore(flags);
}

void usb_rx_notify(void)
{
    irqflags_t flags;
    if (mp_cdc_enabled) {
        while (udi_cdc_is_rx_ready()) {
            uint8_t c;

            // Introduce a critical section to avoid buffer corruption. We use
            // cpu_irq_save instead of cpu_irq_disable because we don't know the
            // current state of IRQs. They may have been turned off already and
            // we don't want to accidentally turn them back on.
            flags = cpu_irq_save();
            // If our buffer is full, then don't get another character otherwise
            // we'll lose a previous character.
            if (usb_rx_count >= USB_RX_BUF_SIZE) {
                cpu_irq_restore(flags);
                break;
            }

            uint8_t current_tail = usb_rx_buf_tail;
            // Pretend we've received a character so that any nested calls to
            // this function have to consider the spot we've reserved.
            if ((USB_RX_BUF_SIZE - 1) == usb_rx_buf_tail) {
                // Reached the end of buffer, revert back to beginning of
                // buffer.
                usb_rx_buf_tail = 0x00;
            } else {
                usb_rx_buf_tail++;
            }
            // The count of characters present in receive buffer is
            // incremented.
            usb_rx_count++;
            // WARNING(tannewt): This call can call us back with the next
            // character!
            c = udi_cdc_getc();

            if (c == interrupt_char) {
                // We consumed a character rather than adding it to the rx
                // buffer so undo the modifications we made to count and the
                // tail.
                usb_rx_count--;
                usb_rx_buf_tail = current_tail;
                cpu_irq_restore(flags);
                mp_keyboard_interrupt();
                // Don't put the interrupt into the buffer, just continue.
                continue;
            }

            // We put the next character where we expected regardless of whether
            // the next character was already loaded in the buffer.
            usb_rx_buf[current_tail] = c;

            cpu_irq_restore(flags);
        }
    }
}

int receive_usb() {
    if (usb_rx_count == 0) {
        return 0;
    }

    // Disable autoreset if someone is using the repl.
    autoreset_disable();

    // Copy from head.
    cpu_irq_disable();
    int data = usb_rx_buf[usb_rx_buf_head];
    usb_rx_buf_head++;
    usb_rx_count--;
    if ((USB_RX_BUF_SIZE) == usb_rx_buf_head) {
      usb_rx_buf_head = 0;
    }
    cpu_irq_enable();

    // Call usb_rx_notify if we just emptied a spot in the buffer.
    if (usb_rx_count == USB_RX_BUF_SIZE - 1) {
         usb_rx_notify();
    }
    return data;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        // Process any mass storage transfers.
        if (mp_msc_enabled) {
            udi_msc_process_trans();
        }
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
        // TODO(tannewt): Switch to callback/interrupt based UART so it can work
        // with the sleepmgr.
        sleepmgr_enter_sleep();
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
    // Always make sure there is enough room in the usb buffer for the outgoing
    // string. If there isn't we risk getting caught in a loop within the usb
    // code as it tries to send all the characters it can't buffer.
    uint32_t start = 0;
    uint32_t start_tick = mp_hal_ticks_ms();
    uint32_t duration = 0;
    if (mp_cdc_enabled) {
        while (start < len && duration < 10) {
            uint8_t buffer_space = udi_cdc_get_free_tx_buffer();
            uint8_t transmit = min(len - start, buffer_space);
            if (transmit > 0) {
                if (udi_cdc_write_buf(str + start, transmit) > 0) {
                    // It didn't transmit successfully so give up.
                    break;
                }
            }
            start += transmit;
            if (mp_msc_enabled) {
                udi_msc_process_trans();
            }
            duration = (mp_hal_ticks_ms() - start_tick) & MP_SMALL_INT_POSITIVE_MASK;
        }
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
    // Process any mass storage transfers.
    // TODO(tannewt): Actually account for how long the processing takes and
    // subtract it from the delay.
    if (mp_msc_enabled) {
        udi_msc_process_trans();
    }
    delay_ms(delay);
}

void mp_hal_delay_us(mp_uint_t delay) {
    // Process any mass storage transfers.
    // TODO(tannewt): Actually account for how long the processing takes and
    // subtract it from the delay.
    if (mp_msc_enabled) {
        udi_msc_process_trans();
    }
    delay_us(delay);
}

// Global millisecond tick count (driven by SysTick interrupt handler).
volatile uint32_t systick_ticks_ms = 0;

void SysTick_Handler(void) {
    // SysTick interrupt handler called when the SysTick timer reaches zero
    // (every millisecond).
    systick_ticks_ms += 1;
    // Keep the counter within the range of 31 bit uint values since that's the
    // max value for micropython 'small' ints.
    systick_ticks_ms = systick_ticks_ms > (0xFFFFFFFF >> 1) ? 0 : systick_ticks_ms;
}

// Interrupt flags that will be saved and restored during disable/Enable
// interrupt functions below.
static irqflags_t irq_flags;

void mp_hal_disable_all_interrupts(void) {
  // Disable all interrupt sources for timing critical sections.
  // Disable ASF-based interrupts.
  irq_flags = cpu_irq_save();
  // Disable SysTick interrupt.
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

void mp_hal_enable_all_interrupts(void) {
  // Enable all interrupt sources after timing critical sections.
  // Restore SysTick interrupt.
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
  // Restore ASF-based interrupts.
  cpu_irq_restore(irq_flags);
}
