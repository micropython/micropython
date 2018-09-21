#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__

// Power
#define NRFX_POWER_ENABLED 1
#define NRFX_POWER_CONFIG_IRQ_PRIORITY 7

// Turn on nrfx supported workarounds for errata in Rev1/Rev2 of nRF52832
#ifdef NRF52832_XXAA
    #define NRFX_SPIS_NRF52_ANOMALY_109_WORKAROUND_ENABLED 1
#endif

// Turn on nrfx supported workarounds for errata in Rev1 of nRF52840
#ifdef NRF52840_XXAA
    #define NRFX_SPIM3_NRF52840_ANOMALY_198_WORKAROUND_ENABLED 1
#endif

// SPI
#define NRFX_SPIM_ENABLED 1

// TWIM0 and TWIM1 are the same peripherals as SPIM0 and SPIM1.
// The IRQ handlers for these peripherals are set up at compile time,
// so out of the box TWIM0/SPIM0 and TWIM1/SPIM1 cannot be shared
// between common-hal/busio/I2C.c and SPI.c.
// We could write an interrupt handler that checks whether it's
// being used for SPI or I2C, but perhaps two I2C's and 1-2 SPI's are good enough for now.

// Enable SPIM2 and SPIM3 (if available)
#define NRFX_SPIM2_ENABLED 1
#ifdef NRF52840_XXAA
    #define NRFX_SPIM3_ENABLED 1
#endif


#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_SPIM_MISO_PULL_CFG 1

// TWI aka. I2C; enable TWIM0 and TWIM1 (no conflict with SPIM choices)
#define NRFX_TWIM_ENABLED 1
#define NRFX_TWIM0_ENABLED 1
#define NRFX_TWIM1_ENABLED 1

#define NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY NRF_TWIM_FREQ_400K
#define NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0

// UART
#define NRFX_UART_ENABLED 1
#define NRFX_UART0_ENABLED 1

#define NRFX_UART_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_UART_DEFAULT_CONFIG_HWFC NRF_UART_HWFC_DISABLED
#define NRFX_UART_DEFAULT_CONFIG_PARITY NRF_UART_PARITY_EXCLUDED
#define NRFX_UART_DEFAULT_CONFIG_BAUDRATE NRF_UART_BAUDRATE_115200

// PWM
#define NRFX_PWM0_ENABLED 1
#define NRFX_PWM1_ENABLED 1
#define NRFX_PWM2_ENABLED 1

#ifdef NRF_PWM3
#define NRFX_PWM3_ENABLED 1
#else
#define NRFX_PWM3_ENABLED 0
#endif

#endif // NRFX_CONFIG_H__
