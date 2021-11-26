#include <stdint.h>
#include "py/obj.h"

void mp_hal_delay_ms(mp_uint_t ms);
void mp_hal_delay_us(mp_uint_t us);
mp_uint_t mp_hal_ticks_cpu(void);
mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_ms(void);

void mp_hal_set_interrupt_char(int c);
int mp_hal_stdin_rx_chr(void);
void mp_hal_stdout_tx_strn(const char *str, size_t len);

typedef enum std_io_t {
    MINI_UART = 0,
    UART,
    UART_QEMU
} std_io_t;

void uart_init(std_io_t interface);

#ifdef MICROPY_HW_USBHOST
#include "usbkbd.h"

void usbkbd_setup();
#endif
