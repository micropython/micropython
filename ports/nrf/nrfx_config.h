#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__

// Power
#define NRFX_POWER_ENABLED 1
#define NRFX_POWER_CONFIG_IRQ_PRIORITY 7

// Turn on nrfx supported workarounds for errata in Rev1/Rev2 of nRF52832
#ifdef NRF52832_XXAA
    #define NRFX_SPIS_NRF52_ANOMALY_109_WORKAROUND_ENABLED 1
#endif

// NOTE: THIS WORKAROUND CAUSES BLE CODE TO CRASH; tested on 2019-03-11.
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
// being used for SPI or I2C, but perhaps two I2C's and 1-2 SPI's are good enough for now.

// Enable SPIM1, SPIM2 and SPIM3 (if available)
// No conflict with TWIM0.
#define NRFX_SPIM1_ENABLED 1
#define NRFX_SPIM2_ENABLED 1
// DON'T ENABLE SPIM3 DUE TO ANOMALY WORKAROUND FAILURE (SEE ABOVE).
// #ifdef NRF52840_XXAA
//     #define NRFX_SPIM_EXTENDED_ENABLED 1
//     #define NRFX_SPIM3_ENABLED 1
// #else
//     #define NRFX_SPIM3_ENABLED 0
// #endif


#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_SPIM_MISO_PULL_CFG 1

// QSPI
#define NRFX_QSPI_ENABLED                          1

// TWI aka. I2C; enable a single bus: TWIM0 (no conflict with SPIM1 and SPIM2)
#define NRFX_TWIM_ENABLED 1
#define NRFX_TWIM0_ENABLED 1
//#define NRFX_TWIM1_ENABLED 1

#define NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY NRF_TWIM_FREQ_400K
#define NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0

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
#define NRFX_TIMER1_ENABLED 1
#define NRFX_TIMER2_ENABLED 1

#ifdef NRFX_TIMER3
#define NRFX_TIMER3_ENABLED 1
#else
#define NRFX_TIMER3_ENABLED 0
#endif

#ifdef NRFX_TIMER4
#define NRFX_TIMER4_ENABLED 1
#else
#define NRFX_TIMER4_ENABLED 0
#endif

#define NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY 7

// GPIO interrupt
#define NRFX_GPIOTE_ENABLED 1
#define NRFX_GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 1
#define NRFX_GPIOTE_CONFIG_IRQ_PRIORITY 7

#endif // NRFX_CONFIG_H__
