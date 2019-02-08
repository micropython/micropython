/*
 * Copyright (c) 2018 Analog Devices, Inc.  All rights reserved.
 *
 * TWI Simple provides a simple, bare metal I2C/TWI driver
 */

#include "bm_twi.h"

/**
 * @brief      Initialize an instance of the TWI driver
 *
 * @param      device        A pointer to the device driver instance
 * @param[in]  address       The TWI / I2C address of the device
 * @param[in]  f_sclk0_freq  The f_sclk0 frequency of the processor
 * @param[in]  device_num    The device number (e.g., TWI0, TWI1, TWI2)
 *
 * @return     { description_of_the_return_value }
 */
BM_TWI_RESULT twi_initialize(BM_TWI *device,
                             uint8_t address,
                             uint32_t f_sclk0_freq,
                             BM_TWI_PERIPHERAL_NUMBER device_num){
    // Memory mapped control registers for each of the three TWI ports
    if (device_num == TWI0) {
        device->pREG_TWI_CTL        = pREG_TWI0_CTL;
        device->pREG_TWI_CLKDIV     = pREG_TWI0_CLKDIV;
        device->pREG_TWI_FIFOCTL    = pREG_TWI0_FIFOCTL;
        device->pREG_TWI_MSTRADDR   = pREG_TWI0_MSTRADDR;
        device->pREG_TWI_ISTAT      = pREG_TWI0_ISTAT;
        device->pREG_TWI_MSTRSTAT   = pREG_TWI0_MSTRSTAT;
        device->pREG_TWI_MSTRCTL    = pREG_TWI0_MSTRCTL;
        device->pREG_TWI_TXDATA8    = pREG_TWI0_TXDATA8;
        device->pREG_TWI_TXDATA16   = pREG_TWI0_TXDATA16;
        device->pREG_TWI_RXDATA8    = pREG_TWI0_RXDATA8;
        device->pREG_TWI_RXDATA16   = pREG_TWI0_RXDATA16;
    }
    else if (device_num == TWI1) {
        device->pREG_TWI_CTL        = pREG_TWI1_CTL;
        device->pREG_TWI_CLKDIV     = pREG_TWI1_CLKDIV;
        device->pREG_TWI_FIFOCTL    = pREG_TWI1_FIFOCTL;
        device->pREG_TWI_MSTRADDR   = pREG_TWI1_MSTRADDR;
        device->pREG_TWI_ISTAT      = pREG_TWI1_ISTAT;
        device->pREG_TWI_MSTRSTAT   = pREG_TWI1_MSTRSTAT;
        device->pREG_TWI_MSTRCTL    = pREG_TWI1_MSTRCTL;
        device->pREG_TWI_TXDATA8    = pREG_TWI1_TXDATA8;
        device->pREG_TWI_TXDATA16   = pREG_TWI1_TXDATA16;
        device->pREG_TWI_RXDATA8    = pREG_TWI1_RXDATA8;
        device->pREG_TWI_RXDATA16   = pREG_TWI1_RXDATA16;
    }
    else if (device_num == TWI2) {
        device->pREG_TWI_CTL        = pREG_TWI2_CTL;
        device->pREG_TWI_CLKDIV     = pREG_TWI2_CLKDIV;
        device->pREG_TWI_FIFOCTL    = pREG_TWI2_FIFOCTL;
        device->pREG_TWI_MSTRADDR   = pREG_TWI2_MSTRADDR;
        device->pREG_TWI_ISTAT      = pREG_TWI2_ISTAT;
        device->pREG_TWI_MSTRSTAT   = pREG_TWI2_MSTRSTAT;
        device->pREG_TWI_MSTRCTL    = pREG_TWI2_MSTRCTL;
        device->pREG_TWI_TXDATA8    = pREG_TWI2_TXDATA8;
        device->pREG_TWI_TXDATA16   = pREG_TWI2_TXDATA16;
        device->pREG_TWI_RXDATA8    = pREG_TWI2_RXDATA8;
        device->pREG_TWI_RXDATA16   = pREG_TWI2_RXDATA16;
    }

    // Check to see that the clock frequency passed is within valid limits
    if (f_sclk0_freq > TWI_SIMPLE_MAX_SCLK0_FREQ) {
        return TWI_SIMPLE_INVALID_SCLK0_FREQ;
    }
    else if (f_sclk0_freq < 10000000) {
        return TWI_SIMPLE_INVALID_SCLK0_FREQ;
    }

    // Set f_sclk0 frequency
    device->f_sclk0 = f_sclk0_freq;

    // Set prescalar to f_sclk0_freq / 10MHz
    device->prescale = (uint8_t)(f_sclk0_freq / 10000000);

    // Set duty cycle
    device->duty_cycle = 0.40;

    // default clock div 100KHz by default
    device->clkdiv = 0x283C;

    // Set device address
    device->address = address;

    // Set temporary address to zero
    device->address_temporary = 0;

    // Reset the TWI controller
    *device->pREG_TWI_CTL      = 0x00;
    *device->pREG_TWI_CTL      = BITM_TWI_CTL_EN |
                                 ((device->prescale << BITP_TWI_CTL_PRESCALE) & BITM_TWI_CTL_PRESCALE) |
                                 0;

    *device->pREG_TWI_MSTRCTL  = 0;

    // Clear status bits
    *device->pREG_TWI_MSTRSTAT = (ENUM_TWI_MSTRSTAT_BUFWRERR_YES |
                                  ENUM_TWI_MSTRSTAT_BUFRDERR_YES |
                                  ENUM_TWI_MSTRSTAT_DNAK_YES |
                                  ENUM_TWI_MSTRSTAT_ANAK_YES |
                                  ENUM_TWI_MSTRSTAT_LOSTARB_YES |
                                  0);

    // Clearing all pending interrupt flags
    *device->pREG_TWI_ISTAT = 0xFF;

    // set clock
    *device->pREG_TWI_CLKDIV = device->clkdiv;

    // Set the device address
    *device->pREG_TWI_MSTRADDR = device->address;

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      Set a temporary address for this TWI/I2C instance
 *
 * When working with A2B controllers like the ad2425w, it is necessary to perform
 * accesses to a second TWI address for that device.  This function sets up a
 * "temporary" address for this instance.  When the accesses using this address are
 * complete, use the twi_restoreAddress to return to using hte original TWI address.
 *
 * @param      device   A pointer to the structure for the device driver
 *                      instance
 * @param[in]  address  The TWI/I2C address
 */
void twi_set_temporary_address(BM_TWI *device,
                               uint8_t address) {

    // Set the device address
    device->address_temporary = address;
}

/**
 * @brief      Restores the original TWI/I2C address for this driver (after
 *             using a temporary address)
 *
 * @param      device  A pointer to the structure for the device driver
 *                     instance
 */
void twi_restore_address(BM_TWI *device) {

    // Set the device address
    device->address_temporary = 0;
}

/**
 * @brief      Single byte write over I2C / TWI / Wire interface
 *
 * @param      device  A pointer to the instance for this driver
 * @param[in]  value   The 8-bit value to write
 */
BM_TWI_RESULT twi_write(BM_TWI *device,
                        uint8_t value) {

    volatile uint32_t timeoutTimer = TWI_TIMEOUT_COUNT;

    // Set/reset the MSTRADDR in case another instance of this driver was writing
    // somewhere else before
    if (device->address_temporary) {
        *device->pREG_TWI_MSTRADDR = device->address_temporary;
    }
    else {
        *device->pREG_TWI_MSTRADDR = device->address;
    }

    // load 8-bit TX data register with value to transmit
    (*device->pREG_TWI_TXDATA8) = value;

    // Enable master transmitter
    (*device->pREG_TWI_MSTRCTL)  =   (1 << 6) |  // set count to a 1
                                   BITM_TWI_MSTRCTL_EN |   // enable
                                   0;

    // wait for transmission to complete
    while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_MCOMP)) {
        if (timeoutTimer-- == 0) {
            return TWI_SIMPLE_TIMEOUT;
        }
    }

    // Clear completion status bit
    (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      Single byte write over I2C / TWI / Wire interface with option to
 *             suppress stop bit (rstart = true)
 *
 * @param      device  A pointer to the instance for this driver
 * @param[in]  value   The value to write
 * @param[in]  rstart  Whether or not to send a stop bit at the end of the
 *                     transmission. If rstart = true, a stop bit will not be
 *                     sent
 */
BM_TWI_RESULT twi_write_r(BM_TWI *device,
                          uint8_t value,
                          bool rstart) {

    volatile uint32_t timeoutTimer = TWI_TIMEOUT_COUNT;

    // Set/reset the MSTRADDR in case another instance of this driver was writing
    // somewhere else before
    if (device->address_temporary) {
        *device->pREG_TWI_MSTRADDR = device->address_temporary;
    }
    else {
        *device->pREG_TWI_MSTRADDR = device->address;
    }

    // Load 8-bit TX data register with value to transmit
    (*device->pREG_TWI_TXDATA8) = value;

    // Enable master transmitter
    (*device->pREG_TWI_MSTRCTL)  =   (1 << 6) |  //< set count to a 1
                                   BITM_TWI_MSTRCTL_EN |   //< enable
                                   0;

    // Check to see if we're suppressing the stop bit after the transmission
    if (rstart) {
        (*device->pREG_TWI_MSTRCTL)  |=  BITM_TWI_MSTRCTL_RSTART; //< repeated start enabled
    }

    // Wait for transmission to complete
    while (!((*device->pREG_TWI_ISTAT & BITM_TWI_ISTAT_MCOMP))) {
        if (timeoutTimer-- == 0) {
            // Flush the FIFO
            (*device->pREG_TWI_FIFOCTL) = 1;
            (*device->pREG_TWI_FIFOCTL) = 0;
            return TWI_SIMPLE_TIMEOUT;
        }
    }

    (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      Multi-byte write over I2c / TWI / Wire.  When rstart = true, the
 *             interface will not send a stop bit at the end of the transfer so
 *             it can be immediately followed by a read.
 *
 *             When writing to control registers on devices connected to this
 *             port, this is typically required when doing a read of a specific
 *             register address.  Write the address with this  rstart = true,
 *             and immediately follow this with a read.
 *
 *             The SHARC's TWI port does support a block transfer but it is
 *             limited in size. It cannot support the larger transfer block
 *             sizes required to send a SigmaDSP initialization block to a
 *             SigmaDSP, for example. Therefore, this routine doesn't rely
 *
 * @param      device  A pointer to the instance for this driver
 * @param      values  A pointer to the buffer to be written
 * @param[in]  count   The number of bytes to write
 * @param[in]  rstart  Whether or not to send a stop bit at the end of the
 *                     transmission. If rstart = true, a stop bit will not be
 *                     sent.
 */
BM_TWI_RESULT twi_write_block_r(BM_TWI *device,
                                uint8_t *values,
                                uint16_t count,
                                bool rstart) {

    int i;
    volatile uint32_t timeoutTimer = TWI_TIMEOUT_COUNT;

    // Set/reset the MSTRADDR in case another instance of this driver was writing
    // somewhere else before
    if (device->address_temporary) {
        *device->pREG_TWI_MSTRADDR = device->address_temporary;
    }
    else {
        *device->pREG_TWI_MSTRADDR = device->address;
    }

    // Flush FIFO in case a previous transfer encountered an error
    (*device->pREG_TWI_FIFOCTL) = 1;
    (*device->pREG_TWI_FIFOCTL) = 0;

    if (count == 0) {

        // Enable master transmitter but without data
        (*device->pREG_TWI_MSTRCTL)  =   (0 << 6) |  // set count to a 1
                                       BITM_TWI_MSTRCTL_EN |   // enable
                                       0;

        // wait for transmission to complete
        while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_MCOMP)) {
            if (timeoutTimer-- == 0) {
                return TWI_SIMPLE_TIMEOUT;
            }
        }

        // Clear completion status bit
        (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;

    }

    else if (count == 1) {
        return twi_write_r(device, values[0],  rstart);
    }

    // We need to do things manually in case block size is greater than 255 (max supported in HW)
    else {
        for (i = 0; i < count - 1; i++) {
            // Load value into TX register
            (*device->pREG_TWI_TXDATA8) = values[i];

            // Set up the transfer
            (*device->pREG_TWI_MSTRCTL)  =   (2 << 6) |  // set count to a 2 but we'll never let it finish
                                           BITM_TWI_MSTRCTL_EN |   // enable
                                           0;

            // Wait for transfer to complete
            timeoutTimer = TWI_TIMEOUT_COUNT;
            while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_TXSERV)) {
                if (timeoutTimer-- == 0) {

                    // Flush FIFO before returning an error
                    (*device->pREG_TWI_FIFOCTL) = 1;
                    (*device->pREG_TWI_FIFOCTL) = 0;

                    return TWI_SIMPLE_TIMEOUT;
                }
            }
            (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_TXSERV;
        }

        // send final write with stop bit
        if (rstart) {
            (*device->pREG_TWI_MSTRCTL)  |=  BITM_TWI_MSTRCTL_RSTART; // repeated start enabled
        }

        (*device->pREG_TWI_TXDATA8) = values[count - 1];

        timeoutTimer = TWI_TIMEOUT_COUNT;
        while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_MCOMP)) {
            if (timeoutTimer-- == 0) {

                // Flush FIFO before returning an error
                (*device->pREG_TWI_FIFOCTL) = 1;
                (*device->pREG_TWI_FIFOCTL) = 0;

                return TWI_SIMPLE_TIMEOUT;
            }
        }
        (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;
    }

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      Standard multi-byte write
 *
 * @param      device  A pointer to the instance for this driver
 * @param      values  A pointer to the buffer to be written
 * @param[in]  count   The number of bytes to write
 */
BM_TWI_RESULT twi_write_block(BM_TWI *device,
                              uint8_t *values,
                              uint16_t count) {
    return twi_write_block_r(device, values, count, false);
}

/**
 * @brief      Standard single-byte TWI read
 *
 * @param      device  A pointer to the instance for this driver
 *
 * @return     returns the 8-bit value that was read
 */
BM_TWI_RESULT twi_read(BM_TWI *device,
                       uint8_t *value) {

    volatile uint32_t timeoutTimer = TWI_TIMEOUT_COUNT;

    // Set/reset the MSTRADDR in case another instance of this driver was writing
    // somewhere else before
    if (device->address_temporary) {
        *device->pREG_TWI_MSTRADDR = device->address_temporary;
    }
    else {
        *device->pREG_TWI_MSTRADDR = device->address;
    }

    // Enable master transmitter
    (*device->pREG_TWI_MSTRCTL)  =   BITM_TWI_MSTRCTL_DIR | // receive mode
                                   (1 << 6) | // set count to a 1
                                   BITM_TWI_MSTRCTL_EN | // enable
                                   0;

    // wait for transmission to complete
    while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_MCOMP)) {
        if (timeoutTimer-- == 0) {
            return TWI_SIMPLE_TIMEOUT;
        }
    }
    (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;

    *value = (*device->pREG_TWI_RXDATA8);

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      Standard single-byte TWI read with the option to suppress the
 *             stop bit at the end of the transacation
 *
 * @param      device  A pointer to the instance for this driver
 * @param[in]  rstart  Whether or not to suppress the stop bit (true = suppress it, false = do not suppress it)
 *
 * @return     returns the 8-bit value that was read
 */
BM_TWI_RESULT twi_read_r(BM_TWI *device,
                         uint8_t *value,
                         bool rstart) {

    volatile uint32_t timeoutTimer = TWI_TIMEOUT_COUNT;

    // Set/reset the MSTRADDR in case another instance of this driver was writing
    // somewhere else before
    if (device->address_temporary) {
        *device->pREG_TWI_MSTRADDR = device->address_temporary;
    }
    else {
        *device->pREG_TWI_MSTRADDR = device->address;
    }

    // Enable master transmitter
    (*device->pREG_TWI_MSTRCTL)  =    BITM_TWI_MSTRCTL_DIR | // receive mode
                                   (1 << 6) | // set count to a 1
                                   BITM_TWI_MSTRCTL_EN | // enable
                                   BITM_TWI_MSTRCTL_RSTART | // do not send stop bit after read
                                   0;

    // wait for transmission to complete
    while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_MCOMP)) {
        if (timeoutTimer-- == 0) {
            return TWI_SIMPLE_TIMEOUT;
        }
    }

    (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;

    *value = (*device->pREG_TWI_RXDATA8);

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      Multi-byte block read with the option to suppress the stop bit at
 *             the end of the transmission
 *
 * @param      device  A pointer to the instance for this driver
 * @param      values  Pointer to an array where this data will be read to
 * @param[in]  count   The number of bytes to be read
 * @param[in]  rstart  Whether or not to suppress the stop bit (true = suppress it, false = do not suppress it)
 */
BM_TWI_RESULT twi_read_block_r(BM_TWI *device,
                               uint8_t *values,
                               uint16_t count,
                               bool rstart) {

    volatile uint32_t timeoutTimer = TWI_TIMEOUT_COUNT;

    // Set/reset the MSTRADDR in case another instance of this driver was writing
    // somewhere else before
    if (device->address_temporary) {
        *device->pREG_TWI_MSTRADDR = device->address_temporary;
    }
    else {
        *device->pREG_TWI_MSTRADDR = device->address;
    }

    int i;
    for (i = 0; i < count - 1; i++) {

        (*device->pREG_TWI_MSTRCTL)  =   BITM_TWI_MSTRCTL_DIR | // receive mode
                                       (2 << 6) | // set count to a 1
                                       BITM_TWI_MSTRCTL_EN | // enable
                                       0;

        // wait for reception to complete
        timeoutTimer = TWI_TIMEOUT_COUNT;
        while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_RXSERV)) {
            if (timeoutTimer-- == 0) {
                return TWI_SIMPLE_TIMEOUT;
            }
        }
        (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_RXSERV;

        values[i] = (*device->pREG_TWI_RXDATA8);
    }
    // send final write with stop bit

    if (rstart) {
        (*device->pREG_TWI_MSTRCTL)  |=  BITM_TWI_MSTRCTL_RSTART; // repeated start enabled
    }

    timeoutTimer = TWI_TIMEOUT_COUNT;
    while (!((*device->pREG_TWI_ISTAT) & BITM_TWI_ISTAT_MCOMP)) {
        if (timeoutTimer-- == 0) {
            return TWI_SIMPLE_TIMEOUT;
        }
    }
    (*device->pREG_TWI_ISTAT) |= BITM_TWI_ISTAT_MCOMP;

    values[count - 1] = (*device->pREG_TWI_RXDATA8);

    return TWI_SIMPLE_SUCCESS;
}

/**
 * @brief      { function_description }
 *
 * @param      device  The device
 * @param      values  The values
 * @param[in]  count   The count
 */
BM_TWI_RESULT twi_read_block(BM_TWI *device,
                             uint8_t *values,
                             uint16_t count) {

    return twi_read_block_r(device, values, count, false);
}

/**
 * @brief      Set the I2C / TWI / Wire clock rate using the actual desired
 *             clock rate (e.g. 100000)
 *
 * @param      device     The device
 * @param[in]  clockFreq  The clock frequency
 */
bool twi_set_clock(BM_TWI *device,
                   uint32_t clockFreq){
    float x = 1 / (float)clockFreq;
    float period = x / (100 * 1E-9);  // divide by 100ns

    float clocksHigh = period * device->duty_cycle;
    float clocksLow = period * (1.0 - device->duty_cycle);

    // TODO: have this return an enum with invalid clock frequency
    if (clocksHigh > 255.0) {
        return false;
    }
    if (clocksLow > 255.0) {
        return false;
    }

    device -> freq = 10000000 / (uint32_t)clocksHigh;

    device->clkdiv = (((uint8_t)clocksHigh) << 8) + (uint8_t)clocksLow;
    (*device->pREG_TWI_CLKDIV) = device->clkdiv;

    return true;
}
