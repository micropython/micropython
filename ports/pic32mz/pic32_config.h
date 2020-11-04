#ifndef MICROPY_INCLUDED_PIC32BIT_CONFIG_H
#define MICROPY_INCLUDED_PIC32BIT_BOARD_H

//64, 100, 124, 144 
#define PIC32_PIN_COUNT 144

/**
 * These defines, sets the initial config of the MCU
 * @Notes : 
 * - Any alternate function, such as UART, SPI or similair, will override the
 *   GPIO config
 */

// GPIO: INPUT, OUTPUT

#if (PIC32_PIN_COUNT != 64)
#define GPIO_RA0            PIC32_GPIO_INPUT 
#define GPIO_RA1            PIC32_GPIO_INPUT 
#define GPIO_RA2            PIC32_GPIO_INPUT 
#define GPIO_RA3            PIC32_GPIO_INPUT 
#define GPIO_RA4            PIC32_GPIO_INPUT 
#define GPIO_RA5            PIC32_GPIO_INPUT 
#define GPIO_RA6            PIC32_GPIO_INPUT 
#define GPIO_RA7            PIC32_GPIO_INPUT 
#define GPIO_RA9            PIC32_GPIO_INPUT 
#define GPIO_RA10           PIC32_GPIO_OUTPUT
#define GPIO_RA14           PIC32_GPIO_INPUT 
#define GPIO_RA15           PIC32_GPIO_INPUT 
#endif

#define GPIO_RB0            PIC32_GPIO_INPUT 
#define GPIO_RB1            PIC32_GPIO_INPUT 
#define GPIO_RB2            PIC32_GPIO_INPUT 
#define GPIO_RB3            PIC32_GPIO_INPUT 
#define GPIO_RB4            PIC32_GPIO_INPUT 
#define GPIO_RB5            PIC32_GPIO_INPUT 
#define GPIO_RB6            PIC32_GPIO_INPUT 
#define GPIO_RB7            PIC32_GPIO_INPUT 
#define GPIO_RB8            PIC32_GPIO_INPUT 
#define GPIO_RB9            PIC32_GPIO_INPUT 
#define GPIO_RB10           PIC32_GPIO_INPUT 
#define GPIO_RB11           PIC32_GPIO_INPUT 
#define GPIO_RB12           PIC32_GPIO_OUTPUT 
#define GPIO_RB13           PIC32_GPIO_OUTPUT 
#define GPIO_RB14           PIC32_GPIO_INPUT 
#define GPIO_RB15           PIC32_GPIO_INPUT 

#if (PIC32_PIN_COUNT != 64)
#define GPIO_RC1            PIC32_GPIO_INPUT 
#define GPIO_RC2            PIC32_GPIO_INPUT 
#define GPIO_RC3            PIC32_GPIO_INPUT 
#define GPIO_RC4            PIC32_GPIO_INPUT 
#endif
#define GPIO_RC12           PIC32_GPIO_INPUT 
#define GPIO_RC13           PIC32_GPIO_INPUT 
#define GPIO_RC14           PIC32_GPIO_INPUT 
#define GPIO_RC15           PIC32_GPIO_INPUT 

#define GPIO_RD0            PIC32_GPIO_INPUT 
#define GPIO_RD1            PIC32_GPIO_INPUT 
#define GPIO_RD2            PIC32_GPIO_INPUT 
#define GPIO_RD3            PIC32_GPIO_INPUT 
#define GPIO_RD4            PIC32_GPIO_INPUT 
#define GPIO_RD5            PIC32_GPIO_INPUT 
#if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
#define GPIO_RD6            PIC32_GPIO_INPUT 
#define GPIO_RD7            PIC32_GPIO_INPUT 
#endif
#define GPIO_RD9            PIC32_GPIO_INPUT 
#define GPIO_RD10           PIC32_GPIO_INPUT 
#define GPIO_RD11           PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 64)
#define GPIO_RD12           PIC32_GPIO_INPUT 
#define GPIO_RD13           PIC32_GPIO_INPUT 
#define GPIO_RD14           PIC32_GPIO_INPUT 
#define GPIO_RD15           PIC32_GPIO_INPUT 
#endif

#define GPIO_RE0            PIC32_GPIO_INPUT 
#define GPIO_RE1            PIC32_GPIO_INPUT 
#define GPIO_RE2            PIC32_GPIO_INPUT 
#define GPIO_RE3            PIC32_GPIO_INPUT 
#define GPIO_RE4            PIC32_GPIO_INPUT 
#define GPIO_RE5            PIC32_GPIO_INPUT 
#define GPIO_RE6            PIC32_GPIO_INPUT 
#define GPIO_RE7            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 64)
#define GPIO_RE8            PIC32_GPIO_INPUT 
#define GPIO_RE9            PIC32_GPIO_INPUT 
#endif

#define GPIO_RF0            PIC32_GPIO_INPUT 
#define GPIO_RF1            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 64)
#define GPIO_RF2            PIC32_GPIO_INPUT 
#endif
#define GPIO_RF3            PIC32_GPIO_INPUT 
#define GPIO_RF4            PIC32_GPIO_INPUT 
#define GPIO_RF5            PIC32_GPIO_INPUT 
#define GPIO_RF8            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 64)
#define GPIO_RF12           PIC32_GPIO_INPUT 
#define GPIO_RF13           PIC32_GPIO_INPUT 
#endif

#if (PIC32_PIN_COUNT != 64)
#define GPIO_RG0            PIC32_GPIO_INPUT 
#define GPIO_RG1            PIC32_GPIO_INPUT 
#endif
#define GPIO_RG6            PIC32_GPIO_INPUT 
#define GPIO_RG7            PIC32_GPIO_INPUT 
#define GPIO_RG8            PIC32_GPIO_INPUT 
#define GPIO_RG9            PIC32_GPIO_INPUT
#if (PIC32_PIN_COUNT != 64)
#define GPIO_RG12           PIC32_GPIO_INPUT 
#define GPIO_RG13           PIC32_GPIO_INPUT 
#define GPIO_RG14           PIC32_GPIO_INPUT 
#define GPIO_RG15           PIC32_GPIO_INPUT
#endif

#if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
#define GPIO_RH0            PIC32_GPIO_INPUT 
#define GPIO_RH1            PIC32_GPIO_INPUT
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RH2            PIC32_GPIO_INPUT 
#define GPIO_RH3            PIC32_GPIO_INPUT 
#endif
#define GPIO_RH4            PIC32_GPIO_INPUT 
#define GPIO_RH5            PIC32_GPIO_INPUT 
#define GPIO_RH6            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RH7            PIC32_GPIO_INPUT 
#endif
#define GPIO_RH8            PIC32_GPIO_INPUT 
#define GPIO_RH9            PIC32_GPIO_INPUT 
#define GPIO_RH10           PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RH11           PIC32_GPIO_INPUT 
#endif
#define GPIO_RH12           PIC32_GPIO_INPUT 
#define GPIO_RH13           PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RH14           PIC32_GPIO_INPUT 
#define GPIO_RH15           PIC32_GPIO_INPUT 
#endif

#define GPIO_RJ0            PIC32_GPIO_INPUT 
#define GPIO_RJ1            PIC32_GPIO_INPUT 
#define GPIO_RJ2            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RJ3            PIC32_GPIO_INPUT 
#endif
#define GPIO_RJ4            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RJ5            PIC32_GPIO_INPUT 
#define GPIO_RJ6            PIC32_GPIO_INPUT 
#define GPIO_RJ7            PIC32_GPIO_INPUT 
#endif
#define GPIO_RJ8            PIC32_GPIO_INPUT 
#define GPIO_RJ9            PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RJ10           PIC32_GPIO_INPUT 
#endif
#define GPIO_RJ11           PIC32_GPIO_INPUT 
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RJ12           PIC32_GPIO_INPUT 
#define GPIO_RJ13           PIC32_GPIO_INPUT 
#define GPIO_RJ14           PIC32_GPIO_INPUT 
#define GPIO_RJ15           PIC32_GPIO_INPUT 
#endif
#if (PIC32_PIN_COUNT != 124)
#define GPIO_RK0            PIC32_GPIO_INPUT 
#define GPIO_RK1            PIC32_GPIO_INPUT 
#define GPIO_RK2            PIC32_GPIO_INPUT 
#define GPIO_RK3            PIC32_GPIO_INPUT 
#define GPIO_RK4            PIC32_GPIO_INPUT 
#define GPIO_RK5            PIC32_GPIO_INPUT 
#define GPIO_RK6            PIC32_GPIO_INPUT 
#define GPIO_RK7            PIC32_GPIO_INPUT 
#endif // 124
#endif // 64 && 100

// TIMERS

#define TIMER_2_ENABLED (0)
#define TIMER_3_ENABLED (0)
#define TIMER_4_ENABLED (0)
#define TIMER_5_ENABLED (0)
#define TIMER_6_ENABLED (0)
#define TIMER_7_ENABLED (0)
#define TIMER_8_ENABLED (0)
#define TIMER_9_ENABLED (0)

#define TIMER1_INTERRUPT_PERIORITY 3
#define TIMER2_INTERRUPT_PERIORITY 4
#define TIMER3_INTERRUPT_PERIORITY 4
#define TIMER4_INTERRUPT_PERIORITY 4
#define TIMER5_INTERRUPT_PERIORITY 4
#define TIMER6_INTERRUPT_PERIORITY 4
#define TIMER7_INTERRUPT_PERIORITY 4
#define TIMER8_INTERRUPT_PERIORITY 4
#define TIMER9_INTERRUPT_PERIORITY 4

// UART

#define PIC32_UART_1_ENABLED (0)
#define PIC32_UART_2_ENABLED (0)
#define PIC32_UART_3_ENABLED (0)
#define PIC32_UART_4_ENABLED (1)
#define PIC32_UART_5_ENABLED (0)
#define PIC32_UART_6_ENABLED (0)

// Pin mapping (from Datasheet)

#define PIC32_UART_4_RX_PIN  pinB14
#define PIC32_UART_4_TX_PIN  pinF12

#define UART_ATTACHED_TO_REPL PIC32_UART_4

#endif // MICROPY_INCLUDED_PIC32BIT_BOARD_H