/*
 * Copyright (c) 2018 Analog Devices, Inc.  All rights reserved.
 */

#ifndef _BM_TWI_H_
#define _BM_TWI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/platform.h>

#define TWI_SIMPLE_MAX_SCLK0_FREQ   (112500000)
#define TWI_TYPICAL_SCLK0_FREQ      (112500000)

#define F_SCK0_FREQUENCY            (F_SCK0)

// Timeout delay for I2C access functions
#define TWI_TIMEOUT_COUNT           (200000)

// Available hardware TWI peripherals
typedef enum _BM_TWI_PERIPHERAL_NUMBER {
    TWI0 = (0),
    TWI1 = (1),
    TWI2 = (2)
} BM_TWI_PERIPHERAL_NUMBER;

// Result of TWI operation
typedef enum _BM_TWI_RESULT
{
    TWI_SIMPLE_SUCCESS,                 // The API call is success
    TWI_SIMPLE_INVALID_DEVICE_NUM,      // Invalid peripheral
    TWI_SIMPLE_TIMEOUT,                 // Access timed out
    TWI_SIMPLE_INVALID_SCLK0_FREQ       // Invalid valid for SCLK
} BM_TWI_RESULT;

// Structure definition for TWI driver
typedef struct _BM_TWI
{

    // Memory-mapped control registers used to program TWI peripheral
    volatile uint16_t *pREG_TWI_CTL;
    volatile uint16_t *pREG_TWI_CLKDIV;
    volatile uint16_t *pREG_TWI_FIFOCTL;
    volatile uint16_t *pREG_TWI_MSTRADDR;
    volatile uint16_t *pREG_TWI_ISTAT;
    volatile uint16_t *pREG_TWI_MSTRSTAT;
    volatile uint16_t *pREG_TWI_MSTRCTL;

    volatile uint16_t *pREG_TWI_TXDATA8;
    volatile uint16_t *pREG_TWI_TXDATA16;
    volatile uint16_t *pREG_TWI_RXDATA8;
    volatile uint16_t *pREG_TWI_RXDATA16;

    uint32_t f_sclk0;
    uint8_t address;                   // The TWI address
    uint8_t address_temporary;         // An alternate TWI address that can be temporarily used (for A2B)
    uint16_t clkdiv;                   // Resulting clock divider
    uint8_t prescale;                  // Resulting clock pre-scalar
    float duty_cycle;                   // TWI clock duty cycle
    uint32_t freq;                     // Frequency cached by twi_set_clock
} BM_TWI;

#ifdef __cplusplus
extern "C" {
#endif

// Initializes an instance of the TWI simple driver
BM_TWI_RESULT twi_initialize(BM_TWI *device,
                             uint8_t address,
                             uint32_t f_sclk0_freq,
                             BM_TWI_PERIPHERAL_NUMBER device_num);

// Writes a byte to the TWI peripheral
BM_TWI_RESULT twi_write(BM_TWI *device,
                        uint8_t value);

// Write a byte to the TWI peripheral with the option to include the rstart bit in the transfer
BM_TWI_RESULT twi_write_r(BM_TWI *device,
                          uint8_t value,
                          bool rstart);

// Writes a block of data to the TWI peripheral
BM_TWI_RESULT twi_write_block(BM_TWI *device,
                              uint8_t *values,
                              uint16_t count);

// Writes a block of data to the TWI peripheral with the option to include the rstart bit in the transfer
BM_TWI_RESULT twi_write_block_r(BM_TWI *device,
                                uint8_t *values,
                                uint16_t count,
                                bool rstart);

// Reads a byte from the TWI peripheral
BM_TWI_RESULT twi_read(BM_TWI *device,
                       uint8_t *value);

// Reads a byte from the TWI peripheral with the option to include the rstart bit in the transfer
BM_TWI_RESULT twi_read_r(BM_TWI *device,
                         uint8_t *value,
                         bool rstart);

// Reads a block of data from the TWI peripheral
BM_TWI_RESULT twi_read_block(BM_TWI *device,
                             uint8_t *values,
                             uint16_t count);

// Reads a block of data from the TWI peripheral with the option to include the rstart bit in the transfer
BM_TWI_RESULT twi_read_block_r(BM_TWI *device,
                               uint8_t *values,
                               uint16_t count,
                               bool rstart);

// Sets the TWI clock
bool twi_set_clock(BM_TWI *device,
                   uint32_t clockFreq);

// Provides a temporary address for this instance (used for A2B slave accesses)
void twi_set_temporary_address(BM_TWI *device,
                               uint8_t address);

// Restores the original TWI address for this instance
void twi_restore_address(BM_TWI *device);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _BM_TWI_H_
