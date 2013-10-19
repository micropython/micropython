#include <stdio.h>

#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

#include "mma.h"

void mma_init() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable I2C1
    //gpio_pin_init(GPIOB, 6 /* B6 is SCL */, 2 /* AF mode */, 1 /* open drain output */, 1 /* 25 MHz */, 0 /* no pull up or pull down */);
    //gpio_pin_init(GPIOB, 7 /* B7 is SDA */, 2 /* AF mode */, 1 /* open drain output */, 1 /* 25 MHz */, 0 /* no pull up or pull down */);
    //gpio_pin_af(GPIOB, 6, 4 /* AF 4 for I2C1 */);
    //gpio_pin_af(GPIOB, 7, 4 /* AF 4 for I2C1 */);
    // XXX untested GPIO init! (was above code)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

    // get clock speeds
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);

    // disable the I2C peripheral before we configure it
    I2C1->CR1 &= ~I2C_CR1_PE;

    // program peripheral input clock
    I2C1->CR2 = 4; // no interrupts; 4 MHz (hopefully!) (could go up to 42MHz)

    // configure clock control reg
    uint32_t freq = rcc_clocks.PCLK1_Frequency / (100000 << 1); // want 100kHz, this is the formula for freq
    I2C1->CCR = freq; // standard mode (speed), freq calculated as above

    // configure rise time reg
    I2C1->TRISE = (rcc_clocks.PCLK1_Frequency / 1000000) + 1; // formula for trise, gives maximum rise time

    // enable the I2C peripheral
    I2C1->CR1 |= I2C_CR1_PE;

    // set START bit in CR1 to generate a start cond!
}

static uint32_t i2c_get_sr() {
    // must read SR1 first, then SR2, as the read can clear some flags
    uint32_t sr1 = I2C1->SR1;
    uint32_t sr2 = I2C1->SR2;
    return (sr2 << 16) | sr1;
}

void mma_restart(uint8_t addr, int write) {
    // send start condition
    I2C1->CR1 |= I2C_CR1_START;

    // wait for BUSY, MSL and SB --> Slave has acknowledged start condition
    while ((i2c_get_sr() & 0x00030001) != 0x00030001) {
    }

    if (write) {
        // send address and write bit
        I2C1->DR = (addr << 1) | 0;
        // wait for BUSY, MSL, ADDR, TXE and TRA
        while ((i2c_get_sr() & 0x00070082) != 0x00070082) {
        }
    } else {
        // send address and read bit
        I2C1->DR = (addr << 1) | 1;
        // wait for BUSY, MSL and ADDR flags
        while ((i2c_get_sr() & 0x00030002) != 0x00030002) {
        }
    }
}

void mma_start(uint8_t addr, int write) {
    // wait until I2C is not busy
    while (I2C1->SR2 & I2C_SR2_BUSY) {
    }

    // do rest of start
    mma_restart(addr, write);
}

void mma_send_byte(uint8_t data) {
    // send byte
    I2C1->DR = data;
    // wait for TRA, BUSY, MSL, TXE and BTF (byte transmitted)
    int timeout = 1000000;
    while ((i2c_get_sr() & 0x00070084) != 0x00070084) {
        if (timeout-- <= 0) {
            printf("mma_send_byte timed out!\n");
            break;
        }
    }
}

uint8_t mma_read_ack() {
    // enable ACK of received byte
    I2C1->CR1 |= I2C_CR1_ACK;
    // wait for BUSY, MSL and RXNE (byte received)
    while ((i2c_get_sr() & 0x00030040) != 0x00030040) {
    }
    // read and return data
    uint8_t data = I2C1->DR;
    return data;
}

uint8_t mma_read_nack() {
    // disable ACK of received byte (to indicate end of receiving)
    I2C1->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
    // last byte should apparently also generate a stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
    // wait for BUSY, MSL and RXNE (byte received)
    while ((i2c_get_sr() & 0x00030040) != 0x00030040) {
    }
    // read and return data
    uint8_t data = I2C1->DR;
    return data;
}

void mma_stop() {
    // send stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
}
