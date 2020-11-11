#include "uart.h"
#include "helpers.h"
#include "sysclk.h"
#include "pic32_config.h"
#include "gpio.h"

#include <xc.h>

#define PIC32_FUNC_U1TX  (0x01)
#define PIC32_FUNC_U2TX  (0x02)
#define PIC32_FUNC_U3TX  (0x01)
#define PIC32_FUNC_U4TX  (0x02)
#define PIC32_FUNC_U5TX  (0x03)
#define PIC32_FUNC_U6TX  (0x04)

#define UMODE_BRGH_BIT    (3)
#define UMODE_RXINV_BIT   (4)
#define UMODE_ON_BIT      (15)
#define UMODE_ABAUD_BIT   (5)

#define UMODE_STSEL_MASK (1 << 0)
#define UMODE_UEN_MASK   ((1 << 8) | (1 << 9))
#define UMODE_PDSEL_MASK ((1 << 1) | (1 << 2))

#define USTA_URXDA_BIT     (0)
#define USTA_OERR_BIT      (1)
#define USTA_FERR_BIT      (2)
#define USTA_PERR_BIT      (3)
#define USTA_RIDLE_BIT     (4)
#define USTA_ADDEN_BIT     (5)
#define USTA_URXISEL_BIT_1 (6)
#define USTA_URXISEL_BIT_2 (7)
#define USTA_TRMT_BIT      (8)
#define USTA_UTXBF_BIT     (9)
#define USTA_UTXEN_BIT     (10)
#define USTA_UTXBRK_BIT    (11)
#define USTA_URXEN_BIT     (12)
#define USTA_UTXINV_BIT    (13)
#define USTA_UTXISEL_BIT_1 (14)
#define USTA_UTXISEL_BIT_2 (15)

#define USTA_URXDA_MASK   (1 << USTA_URXDA_BIT)
#define USTA_OERR_MASK    (1 << USTA_OERR_BIT)
#define USTA_FERR_MASK    (1 << USTA_FERR_BIT)
#define USTA_PERR_MASK    (1 << USTA_PERR_BIT)
#define USTA_RIDLE_MASK   (1 << USTA_RIDLE_BIT)
#define USTA_ADDEN_MASK   (1 << USTA_ADDEN_BIT)
#define USTA_URXISEL_MASK ((1 << USTA_URXISEL_BIT_1) | (1 << USTA_URXISEL_BIT_2))
#define USTA_TRMT_MASK    (1 << USTA_TRMT_BIT)
#define USTA_UTXBF_MASK   (1 << USTA_UTXBF_BIT)
#define USTA_UTXEN_MASK   (1 << USTA_UTXEN_BIT)
#define USTA_UTXBRK_MASK  (1 << USTA_UTXBRK_BIT)
#define USTA_URXEN_MASK   (1 << USTA_URXEN_BIT)
#define USTA_UTXINV_MASK  (1 << USTA_UTXINV_BIT)
#define USTA_UTXISEL_MASK ((1 << USTA_UTXISEL_BIT_1) | (1 << USTA_UTXISEL_BIT_2))


static uint32_t volatile *const uart_mode[] =
{
    &U1MODE,
    &U2MODE,
    &U3MODE,
    &U4MODE,
    &U5MODE,
    &U6MODE,
};

static uint32_t volatile *const uart_stat[] =
{
    &U1STA,
    &U2STA,
    &U3STA,
    &U4STA,
    &U5STA,
    &U6STA,
};

static uint32_t volatile *const uart_brg[] =
{
    &U1BRG,
    &U2BRG,
    &U3BRG,
    &U4BRG,
    &U5BRG,
    &U6BRG,
};

static uint32_t volatile *const uart_rxreg[] =
{
    &U1RXREG,
    &U2RXREG,
    &U3RXREG,
    &U4RXREG,
    &U5RXREG,
    &U6RXREG,
};

static uint32_t volatile *const uart_txreg[] =
{
    &U1TXREG,
    &U2TXREG,
    &U3TXREG,
    &U4TXREG,
    &U5TXREG,
    &U6TXREG,
};

typedef void (*uart_setpins_ft)(void);

#if (PIC32_UART_1_ENABLED == 1)
static inline void set_uart1_pinmap(void);
#endif

#if (PIC32_UART_2_ENABLED == 1)
static inline void set_uart2_pinmap(void);
#endif

#if (PIC32_UART_3_ENABLED == 1)
static inline void set_uart3_pinmap(void);
#endif

#if (PIC32_UART_4_ENABLED == 1)
static inline void set_uart4_pinmap(void);
#endif

#if (PIC32_UART_5_ENABLED == 1)
static inline void set_uart5_pinmap(void);
#endif

#if (PIC32_UART_6_ENABLED == 1)
static inline void set_uart6_pinmap(void);
#endif

static uart_setpins_ft pic32_uart_setpins[] =
{
    #if (PIC32_UART_1_ENABLED == 1)
    set_uart1_pinmap,
    #else
    NULL,
    #endif
    #if (PIC32_UART_2_ENABLED == 1)
    set_uart2_pinmap,
    #else
    NULL,
    #endif
    #if (PIC32_UART_3_ENABLED == 1)
    set_uart3_pinmap,
    #else
    NULL,
    #endif
    #if (PIC32_UART_4_ENABLED == 1)
    set_uart4_pinmap,
    #else
    NULL,
    #endif
    #if (PIC32_UART_5_ENABLED == 1)
    set_uart5_pinmap,
    #else
    NULL,
    #endif
    #if (PIC32_UART_6_ENABLED == 1)
    set_uart6_pinmap,
    #else
    NULL,
    #endif
};

void uart_init(pic32_uart_t uart_id,
    pic32_uart_parity_data_t pdata,
    pic32_uart_stop_bit_t stop_bit,
    uint32_t baud) {
    uint32_t pb_uart_clk = sysclk_uartfreq_get();

    if (pic32_uart_setpins[uart_id] != NULL) {
        pic32_uart_setpins[uart_id]();
    } else {
        return;
    }

    *uart_mode[uart_id] = 0;

    BIT_CLR(uart_mode[uart_id], UMODE_BRGH_BIT);

    *uart_brg[uart_id] = pb_uart_clk / (16 * baud) - 1;

    *uart_stat[uart_id] = 0; // disable Tx Rx, clear all flags
    BIT_SET(uart_stat[uart_id], USTA_UTXEN_BIT);
    BIT_SET(uart_stat[uart_id], USTA_URXEN_BIT);

    BIT_MASK(uart_mode[uart_id], UMODE_PDSEL_MASK, pdata);
    BIT_MASK(uart_mode[uart_id], UMODE_STSEL_MASK, stop_bit);

    BIT_SET(uart_mode[uart_id], UMODE_ON_BIT);
}

int uart_rx_ready(pic32_uart_t uart_id) {
    return (int)(*uart_stat[uart_id]) & USTA_URXDA_MASK;
}

int uart_read_char(pic32_uart_t uart_id) {
    return (int)(*uart_rxreg[uart_id]);
}


void uart_send_char(pic32_uart_t uart_id, int c) {
    while (((*uart_stat[uart_id]) & USTA_UTXBF_MASK) != 0) {
        ;
    }
    (*uart_txreg[uart_id]) = (char)c;
}

#if (PIC32_UART_1_ENABLED == 1)
static inline void set_uart1_pinmap(void) {
    uint8_t func_index;
    func_index = gpio_map_getindex(PIC32_UART_1_RX_PIN);

    if (func_index != INPUT_PIN_NC) {
        func_index = func_index % PIC32_MAX_PORT_PIN;
        U1RXR = func_index;
    } else {
        // wrong mapping value
    }
    func_index = gpio_map_getindex(PIC32_UART_1_TX_PIN);

    gpio_outfunc_map_set(func_index, PIC32_FUNC_U1TX);

    gpio_input_set(PIC32_UART_1_RX_PIN);

    gpio_input_set(PIC32_UART_1_RX_PIN);
}
#endif

#if (PIC32_UART_2_ENABLED == 1)
static inline void set_uart2_pinmap(void) {
    uint8_t func_index;
    func_index = gpio_map_getindex(PIC32_UART_2_RX_PIN);

    if (func_index != INPUT_PIN_NC) {
        func_index = func_index % PIC32_MAX_PORT_PIN;
        U2RXR = func_index;
    } else {
        // wrong mapping value
    }
    func_index = gpio_map_getindex(PIC32_UART_2_TX_PIN);

    gpio_outfunc_map_set(func_index, PIC32_FUNC_U2TX);

    gpio_input_set(PIC32_UART_2_RX_PIN);

    gpio_input_set(PIC32_UART_2_RX_PIN);
}
#endif

#if (PIC32_UART_3_ENABLED == 1)
static inline void set_uart3_pinmap(void) {
    uint8_t func_index;
    func_index = gpio_map_getindex(PIC32_UART_3_RX_PIN);

    if (func_index != INPUT_PIN_NC) {
        func_index = func_index % PIC32_MAX_PORT_PIN;
        U3RXR = func_index;
    } else {
        // wrong mapping value
    }
    func_index = gpio_map_getindex(PIC32_UART_3_TX_PIN);

    gpio_outfunc_map_set(func_index, PIC32_FUNC_U3TX);

    gpio_input_set(PIC32_UART_3_RX_PIN);

    gpio_input_set(PIC32_UART_3_RX_PIN);
}
#endif

#if (PIC32_UART_4_ENABLED == 1)
static inline void set_uart4_pinmap(void) {
    uint8_t func_index;
    func_index = gpio_map_getindex(PIC32_UART_4_RX_PIN);

    if (func_index != INPUT_PIN_NC) {
        func_index = func_index % PIC32_MAX_PORT_PIN;
        U4RXR = func_index;
    } else {
        // wrong mapping value
    }

    func_index = gpio_map_getindex(PIC32_UART_4_TX_PIN);

    gpio_outfunc_map_set(func_index, PIC32_FUNC_U4TX);

    gpio_input_set(PIC32_UART_4_RX_PIN);
}
#endif

#if (PIC32_UART_5_ENABLED == 1)
static inline void set_uart5_pinmap(void) {
    uint8_t func_index;
    func_index = gpio_map_getindex(PIC32_UART_5_RX_PIN);

    if (func_index != INPUT_PIN_NC) {
        func_index = func_index % PIC32_MAX_PORT_PIN;
        U5RXR = func_index;
    } else {
        // wrong mapping value
    }
    func_index = gpio_map_getindex(PIC32_UART_5_TX_PIN);

    gpio_outfunc_map_set(func_index, PIC32_FUNC_U5TX);

    gpio_input_set(PIC32_UART_5_RX_PIN);

    gpio_input_set(PIC32_UART_5_RX_PIN);
}
#endif

#if (PIC32_UART_6_ENABLED == 1)
static inline void set_uart6_pinmap(void) {
    uint8_t func_index;
    func_index = gpio_map_getindex(PIC32_UART_6_RX_PIN);

    if (func_index != INPUT_PIN_NC) {
        func_index = func_index % PIC32_MAX_PORT_PIN;
        U6RXR = func_index;
    } else {
        // wrong mapping value
    }
    func_index = gpio_map_getindex(PIC32_UART_6_TX_PIN);

    gpio_outfunc_map_set(func_index, PIC32_FUNC_U6TX);

    gpio_input_set(PIC32_UART_6_RX_PIN);

    gpio_input_set(PIC32_UART_6_RX_PIN);
}
#endif