/*
* The MIT License (MIT)
*
* Copyright (c) 2015 David Ogilvy (MetalPhreak)
* Modified 2016 by Radomir Dopieralski
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "hspi.h"

/*
Wrapper to setup HSPI/SPI GPIO pins and default SPI clock
    spi_no - SPI (0) or HSPI (1)
Not used in Micropython.
*/
void spi_init(uint8_t spi_no) {
    spi_init_gpio(spi_no, SPI_CLK_USE_DIV);
    spi_clock(spi_no, SPI_CLK_PREDIV, SPI_CLK_CNTDIV);
    spi_tx_byte_order(spi_no, SPI_BYTE_ORDER_HIGH_TO_LOW);
    spi_rx_byte_order(spi_no, SPI_BYTE_ORDER_HIGH_TO_LOW);

    SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_CS_SETUP|SPI_CS_HOLD);
    CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_FLASH_MODE);
}


/*
Configures SPI mode parameters for clock edge and clock polarity.
    spi_no - SPI (0) or HSPI (1)
    spi_cpha - (0) Data is valid on clock leading edge
               (1) Data is valid on clock trailing edge
    spi_cpol - (0) Clock is low when inactive
               (1) Clock is high when inactive
For Micropython this version is different from original.
*/
void spi_mode(uint8_t spi_no, uint8_t spi_cpha, uint8_t spi_cpol) {
    if (spi_cpol) {
        SET_PERI_REG_MASK(SPI_PIN(HSPI), SPI_IDLE_EDGE);
    } else {
        CLEAR_PERI_REG_MASK(SPI_PIN(HSPI), SPI_IDLE_EDGE);
    }
    if (spi_cpha == spi_cpol) {
        // Mode 3 - MOSI is set on falling edge of clock
        // Mode 0 - MOSI is set on falling edge of clock
        CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_CK_OUT_EDGE);
        SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_CK_I_EDGE);
    } else {
        // Mode 2 - MOSI is set on rising edge of clock
        // Mode 1 - MOSI is set on rising edge of clock
        SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_CK_OUT_EDGE);
        CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_CK_I_EDGE);
    }
}


/*
Initialise the GPIO pins for use as SPI pins.
    spi_no - SPI (0) or HSPI (1)
    sysclk_as_spiclk -
        SPI_CLK_80MHZ_NODIV (1) if using 80MHz for SPI clock.
        SPI_CLK_USE_DIV     (0) if using divider for lower speed.
*/
void spi_init_gpio(uint8_t spi_no, uint8_t sysclk_as_spiclk) {
    uint32_t clock_div_flag = 0;
    if (sysclk_as_spiclk) {
        clock_div_flag = 0x0001;
    }
    if (spi_no == SPI) {
        // Set bit 8 if 80MHz sysclock required
        WRITE_PERI_REG(PERIPHS_IO_MUX, 0x005 | (clock_div_flag<<8));
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, 1);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, 1);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U, 1);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U, 1);
    } else if (spi_no == HSPI) {
        // Set bit 9 if 80MHz sysclock required
        WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105 | (clock_div_flag<<9));
        // GPIO12 is HSPI MISO pin (Master Data In)
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);
        // GPIO13 is HSPI MOSI pin (Master Data Out)
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);
        // GPIO14 is HSPI CLK pin (Clock)
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);
        // GPIO15 is HSPI CS pin (Chip Select / Slave Select)
        // In Micropython, we are handling CS ourself in drivers.
        // PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);
    }
}


/*
Set up the control registers for the SPI clock
    spi_no - SPI (0) or HSPI (1)
    prediv - predivider value (actual division value)
    cntdiv - postdivider value (actual division value)
Set either divider to 0 to disable all division (80MHz sysclock)
*/
void spi_clock(uint8_t spi_no, uint16_t prediv, uint8_t cntdiv) {
    if (prediv == 0 || cntdiv == 0) {
        WRITE_PERI_REG(SPI_CLOCK(spi_no), SPI_CLK_EQU_SYSCLK);
    } else {
        WRITE_PERI_REG(SPI_CLOCK(spi_no),
           (((prediv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
           (((cntdiv - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
           (((cntdiv >> 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
           ((0 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S)
        );
    }
}


/*
Setup the byte order for shifting data out of buffer
    spi_no - SPI (0) or HSPI (1)
    byte_order -
        SPI_BYTE_ORDER_HIGH_TO_LOW (1)
            Data is sent out starting with Bit31 and down to Bit0
        SPI_BYTE_ORDER_LOW_TO_HIGH (0)
            Data is sent out starting with the lowest BYTE, from MSB to LSB,
            followed by the second lowest BYTE, from MSB to LSB, followed by
            the second highest BYTE, from MSB to LSB, followed by the highest
            BYTE, from MSB to LSB 0xABCDEFGH would be sent as 0xGHEFCDAB.
*/
void spi_tx_byte_order(uint8_t spi_no, uint8_t byte_order) {
    if (byte_order) {
        SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_WR_BYTE_ORDER);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_WR_BYTE_ORDER);
    }
}


/*
Setup the byte order for shifting data into buffer
    spi_no - SPI (0) or HSPI (1)
    byte_order -
        SPI_BYTE_ORDER_HIGH_TO_LOW (1)
            Data is read in starting with Bit31 and down to Bit0
        SPI_BYTE_ORDER_LOW_TO_HIGH (0)
            Data is read in starting with the lowest BYTE, from MSB to LSB,
            followed by the second lowest BYTE, from MSB to LSB, followed by
            the second highest BYTE, from MSB to LSB, followed by the highest
            BYTE, from MSB to LSB 0xABCDEFGH would be read as 0xGHEFCDAB
*/
void spi_rx_byte_order(uint8_t spi_no, uint8_t byte_order) {
    if (byte_order) {
        SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_RD_BYTE_ORDER);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_RD_BYTE_ORDER);
    }
}


/*
SPI transaction function
    spi_no - SPI (0) or HSPI (1)
    cmd_bits - actual number of bits to transmit
    cmd_data - command data
    addr_bits - actual number of bits to transmit
    addr_data - address data
    dout_bits - actual number of bits to transmit
    dout_data - output data
    din_bits - actual number of bits to receive
Returns: read data - uint32_t containing read in data only if RX was set
    0 - something went wrong (or actual read data was 0)
    1 - data sent ok (or actual read data is 1)
Note: all data is assumed to be stored in the lower bits of the data variables
(for anything <32 bits).
*/
uint32_t spi_transaction(uint8_t spi_no, uint8_t cmd_bits, uint16_t cmd_data,
                         uint32_t addr_bits, uint32_t addr_data,
                         uint32_t dout_bits, uint32_t dout_data,
                         uint32_t din_bits, uint32_t dummy_bits) {
    while (spi_busy(spi_no)) {};  // Wait for SPI to be ready

// Enable SPI Functions
    // Disable MOSI, MISO, ADDR, COMMAND, DUMMY in case previously set.
    CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_MOSI | SPI_USR_MISO |
                        SPI_USR_COMMAND | SPI_USR_ADDR | SPI_USR_DUMMY);

    // Enable functions based on number of bits. 0 bits = disabled.
    // This is rather inefficient but allows for a very generic function.
    // CMD ADDR and MOSI are set below to save on an extra if statement.
    if (din_bits) {
        if (dout_bits) {
            SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_DOUTDIN);
        } else {
            SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_MISO);
        }
    }
    if (dummy_bits) {
        SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_DUMMY);
    }

// Setup Bitlengths
    WRITE_PERI_REG(SPI_USER1(spi_no),
        // Number of bits in Address
        ((addr_bits - 1) & SPI_USR_ADDR_BITLEN) << SPI_USR_ADDR_BITLEN_S |
        // Number of bits to Send
        ((dout_bits - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S |
        // Number of bits to receive
        ((din_bits - 1) & SPI_USR_MISO_BITLEN) << SPI_USR_MISO_BITLEN_S |
        // Number of Dummy bits to insert
        ((dummy_bits - 1) & SPI_USR_DUMMY_CYCLELEN) << SPI_USR_DUMMY_CYCLELEN_S);

// Setup Command Data
    if (cmd_bits) {
        // Enable COMMAND function in SPI module
        SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_COMMAND);
        // Align command data to high bits
        uint16_t command = cmd_data << (16-cmd_bits);
        // Swap byte order
        command = ((command>>8)&0xff) | ((command<<8)&0xff00);
        WRITE_PERI_REG(SPI_USER2(spi_no), (
            (((cmd_bits - 1) & SPI_USR_COMMAND_BITLEN) << SPI_USR_COMMAND_BITLEN_S) |
            (command & SPI_USR_COMMAND_VALUE)
        ));
    }

// Setup Address Data
    if (addr_bits) {
        // Enable ADDRess function in SPI module
        SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_ADDR);
        // Align address data to high bits
        WRITE_PERI_REG(SPI_ADDR(spi_no), addr_data << (32 - addr_bits));
    }

// Setup DOUT data
    if (dout_bits) {
        // Enable MOSI function in SPI module
        SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_MOSI);
    // Copy data to W0
    if (READ_PERI_REG(SPI_USER(spi_no))&SPI_WR_BYTE_ORDER) {
        WRITE_PERI_REG(SPI_W0(spi_no), dout_data << (32 - dout_bits));
    } else {
        uint8_t dout_extra_bits = dout_bits%8;

        if (dout_extra_bits) {
            // If your data isn't a byte multiple (8/16/24/32 bits) and you
            // don't have SPI_WR_BYTE_ORDER set, you need this to move the
            // non-8bit remainder to the MSBs. Not sure if there's even a use
            // case for this, but it's here if you need it... For example,
            // 0xDA4 12 bits without SPI_WR_BYTE_ORDER would usually be output
            // as if it were 0x0DA4, of which 0xA4, and then 0x0 would be
            // shifted out (first 8 bits of low byte, then 4 MSB bits of high
            // byte - ie reverse byte order).
            // The code below shifts it out as 0xA4 followed by 0xD as you
            // might require.
            WRITE_PERI_REG(SPI_W0(spi_no), (
                (0xFFFFFFFF << (dout_bits - dout_extra_bits) & dout_data)
                    << (8-dout_extra_bits) |
                ((0xFFFFFFFF >> (32 - (dout_bits - dout_extra_bits)))
                    & dout_data)
            ));
        } else {
            WRITE_PERI_REG(SPI_W0(spi_no), dout_data);
        }
    }
}

// Begin SPI Transaction
    SET_PERI_REG_MASK(SPI_CMD(spi_no), SPI_USR);

// Return DIN data
    if (din_bits) {
        while (spi_busy(spi_no)) {}; // Wait for SPI transaction to complete
        if (READ_PERI_REG(SPI_USER(spi_no))&SPI_RD_BYTE_ORDER) {
            // Assuming data in is written to MSB. TBC
            return READ_PERI_REG(SPI_W0(spi_no)) >> (32 - din_bits);
        } else {
            // Read in the same way as DOUT is sent. Note existing contents of
            // SPI_W0 remain unless overwritten!
            return READ_PERI_REG(SPI_W0(spi_no));
        }
        return 0; // Something went wrong
    }

    // Transaction completed
    return 1; // Success
}


/*
Just do minimal work needed to send 8 bits.
*/
inline void spi_tx8fast(uint8_t spi_no, uint8_t dout_data) {
    while (spi_busy(spi_no)) {};  // Wait for SPI to be ready

// Enable SPI Functions
    // Disable MOSI, MISO, ADDR, COMMAND, DUMMY in case previously set.
    CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_MOSI | SPI_USR_MISO |
                        SPI_USR_COMMAND | SPI_USR_ADDR | SPI_USR_DUMMY);

// Setup Bitlengths
    WRITE_PERI_REG(SPI_USER1(spi_no),
        // Number of bits to Send
        ((8 - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S |
        // Number of bits to receive
        ((8 - 1) & SPI_USR_MISO_BITLEN) << SPI_USR_MISO_BITLEN_S);


// Setup DOUT data
    // Enable MOSI function in SPI module
    SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_MOSI);
    // Copy data to W0
    if (READ_PERI_REG(SPI_USER(spi_no)) & SPI_WR_BYTE_ORDER) {
        WRITE_PERI_REG(SPI_W0(spi_no), dout_data << (32 - 8));
    } else {
        WRITE_PERI_REG(SPI_W0(spi_no), dout_data);
    }

// Begin SPI Transaction
    SET_PERI_REG_MASK(SPI_CMD(spi_no), SPI_USR);
}
