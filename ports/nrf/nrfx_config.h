#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__

// Power
#define NRFX_POWER_ENABLED 1
#define NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY 7

// NOTE: THIS WORKAROUND CAUSES BLE CODE TO CRASH. DO NOT USE.
// It doesn't work with the SoftDevice.
// See https://devzone.nordicsemi.com/f/nordic-q-a/33982/sdk-15-software-crash-during-spi-session
// Turn on nrfx supported workarounds for errata in Rev1 of nRF52840
#ifdef NRF52840_XXAA
//    #define NRFX_SPIM3_NRF52840_ANOMALY_198_WORKAROUND_ENABLED 1
#endif

// SPI
#define NRFX_SPIM_ENABLED 1

// TWIM0 and TWIM1 are the same peripherals as SPIM0 and SPIM1.
// The IRQ handlers for these peripherals are set up at compile time,
// so out of the box TWIM0/SPIM0 and TWIM1/SPIM1 cannot be shared
// between common-hal/busio/I2C.c and SPI.c.
// We could write an interrupt handler that checks whether it's
// being used for SPI or I2C, but perhaps one I2C and two SPI or two I2C and one SPI
// are good enough for now.

// CIRCUITPY_NRF_NUM_I2C is 1 or 2 to choose how many I2C (TWIM) peripherals
// to provide.
// With SPIM3 working we can have two I2C and two SPI.
#ifndef CIRCUITPY_NRF_NUM_I2C
#define CIRCUITPY_NRF_NUM_I2C 2
#endif

#if CIRCUITPY_NRF_NUM_I2C != 0 && CIRCUITPY_NRF_NUM_I2C != 1 && CIRCUITPY_NRF_NUM_I2C != 2
#error CIRCUITPY_NRF_NUM_I2C must be 0, 1, or 2
#endif

// Enable SPIM1, SPIM2 and SPIM3 (if available)
// No conflict with TWIM0.
#if CIRCUITPY_NRF_NUM_I2C == 1
#define NRFX_SPIM1_ENABLED 1
#endif
#define NRFX_SPIM2_ENABLED 1
#ifndef NRFX_SPIM3_ENABLED
#if defined(NRF52840_XXAA) || defined(NRF52833_XXAA)
    #define NRFX_SPIM_EXTENDED_ENABLED 1
    #define NRFX_SPIM3_ENABLED 1
#elif CIRCUITPY_NRF_NUM_I2C == 2
    #define NRFX_SPIM3_ENABLED 0
#endif
#endif


#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_SPIM_MISO_PULL_CFG 1

// QSPI
#if defined(NRF52840_XXAA)
#define NRFX_QSPI_ENABLED                          1
#endif

// TWI aka. I2C; always enable TWIM0 (no conflict with SPIM1 and SPIM2)
#if CIRCUITPY_NRF_NUM_I2C == 1 || CIRCUITPY_NRF_NUM_I2C == 2
#define NRFX_TWIM_ENABLED 1
#define NRFX_TWIM0_ENABLED 1
#define NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY NRF_TWIM_FREQ_400K
#define NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
#endif

#if CIRCUITPY_NRF_NUM_I2C == 2
#define NRFX_TWIM1_ENABLED 1
#endif

// UART
#define NRFX_UARTE_ENABLED 1
#define NRFX_UARTE0_ENABLED 1
#define NRFX_UARTE1_ENABLED 1

// PWM
#define NRFX_PWM0_ENABLED 1
#define NRFX_PWM1_ENABLED 1
#define NRFX_PWM2_ENABLED 1

#ifdef NRF_PWM3
#define NRFX_PWM3_ENABLED 1
#else
#define NRFX_PWM3_ENABLED 0
#endif

#define NRFX_RTC_ENABLED 1
#define NRFX_RTC0_ENABLED 1
#define NRFX_RTC1_ENABLED 1
#define NRFX_RTC2_ENABLED 1

// TIMERS
#define NRFX_TIMER_ENABLED 1
// Don't enable TIMER0: it's used by the SoftDevice.
#define NRFX_TIMER0_ENABLED 0
#define NRFX_TIMER1_ENABLED 1
#define NRFX_TIMER2_ENABLED 1

#ifdef NRF_TIMER3
#define NRFX_TIMER3_ENABLED 1
#else
#define NRFX_TIMER3_ENABLED 0
#endif

#ifdef NRF_TIMER4
#define NRFX_TIMER4_ENABLED 1
#else
#define NRFX_TIMER4_ENABLED 0
#endif

#define NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY 7

// GPIO interrupt
#define NRFX_GPIOTE_ENABLED 1
#define NRFX_GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 2
#define NRFX_GPIOTE_CONFIG_IRQ_PRIORITY 7

// NVM controller
#define NRFX_NVMC_ENABLED 1

// Watchdog timer
#define NRFX_WDT_ENABLED 1
#define NRFX_WDT0_ENABLED 1
// This IRQ indicates the system will reboot shortly, so give
// it a high priority.
#define NRFX_WDT_DEFAULT_CONFIG_IRQ_PRIORITY 2

#endif // NRFX_CONFIG_H__
