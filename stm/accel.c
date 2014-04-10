#include <stdio.h>

#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "systick.h"
#include "obj.h"
#include "runtime.h"
#include "accel.h"

#define ACCEL_ADDR (0x4c)

void accel_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable I2C1

    //gpio_pin_init(GPIOB, 6 /* B6 is SCL */, 2 /* AF mode */, 1 /* open drain output */, 1 /* 25 MHz */, 0 /* no pull up or pull down */);
    //gpio_pin_init(GPIOB, 7 /* B7 is SDA */, 2 /* AF mode */, 1 /* open drain output */, 1 /* 25 MHz */, 0 /* no pull up or pull down */);
    //gpio_pin_af(GPIOB, 6, 4 /* AF 4 for I2C1 */);
    //gpio_pin_af(GPIOB, 7, 4 /* AF 4 for I2C1 */);
    // XXX untested GPIO init! (was above code)

    GPIO_InitTypeDef GPIO_InitStructure;

    // PB5 is connected to AVDD; pull high to enable MMA accel device
    GPIOB->BSRRH = GPIO_Pin_5; // PB5 low to start with
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB6=SCL, PB7=SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // alternate functions for SCL and SDA
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

    // wait 20ms, then turn on AVDD, then wait 20ms again; this seems to work, but maybe can decrease delays
    // doesn't work for soft reboot; 50ms doesn't work either...
    sys_tick_delay_ms(20);
    GPIOB->BSRRL = GPIO_Pin_5;
    sys_tick_delay_ms(20);

    // set START bit in CR1 to generate a start cond!

    // init the chip via I2C commands
    accel_start(ACCEL_ADDR, 1);
    accel_send_byte(0);
    accel_stop();

    /*
    // read and print all 11 registers
    accel_start(ACCEL_ADDR, 1);
    accel_send_byte(0);
    accel_restart(ACCEL_ADDR, 0);
    for (int i = 0; i <= 0xa; i++) {
        int data;
        if (i == 0xa) {
            data = accel_read_nack();
        } else {
            data = accel_read_ack();
        }
        printf(" %02x", data);
    }
    printf("\n");
    */

    // put into active mode
    accel_start(ACCEL_ADDR, 1);
    accel_send_byte(7); // mode
    accel_send_byte(1); // active mode
    accel_stop();

    /*
    // infinite loop to read values
    for (;;) {
        sys_tick_delay_ms(500);

        accel_start(ACCEL_ADDR, 1);
        accel_send_byte(0);
        accel_restart(ACCEL_ADDR, 0);
        for (int i = 0; i <= 3; i++) {
            int data;
            if (i == 3) {
                data = accel_read_nack();
                printf(" %02x\n", data);
            } else {
                data = accel_read_ack() & 0x3f;
                if (data & 0x20) {
                    data |= ~0x1f;
                }
                printf(" % 2d", data);
            }
        }
    }
    */
}

static uint32_t i2c_get_sr(void) {
    // must read SR1 first, then SR2, as the read can clear some flags
    uint32_t sr1 = I2C1->SR1;
    uint32_t sr2 = I2C1->SR2;
    return (sr2 << 16) | sr1;
}

void accel_restart(uint8_t addr, int write) {
    // send start condition
    I2C1->CR1 |= I2C_CR1_START;

    // wait for BUSY, MSL and SB --> Slave has acknowledged start condition
    uint32_t timeout = 1000000;
    while ((i2c_get_sr() & 0x00030001) != 0x00030001) {
        if (--timeout == 0) {
            printf("timeout in accel_restart\n");
            return;
        }
    }

    if (write) {
        // send address and write bit
        I2C1->DR = (addr << 1) | 0;
        // wait for BUSY, MSL, ADDR, TXE and TRA
        timeout = 1000000;
        while ((i2c_get_sr() & 0x00070082) != 0x00070082) {
            if (--timeout == 0) {
                printf("timeout in accel_restart write\n");
                return;
            }
        }
    } else {
        // send address and read bit
        I2C1->DR = (addr << 1) | 1;
        // wait for BUSY, MSL and ADDR flags
        timeout = 1000000;
        while ((i2c_get_sr() & 0x00030002) != 0x00030002) {
            if (--timeout == 0) {
                printf("timeout in accel_restart read\n");
                return;
            }
        }
    }
}

void accel_start(uint8_t addr, int write) {
    // wait until I2C is not busy
    uint32_t timeout = 1000000;
    while (I2C1->SR2 & I2C_SR2_BUSY) {
        if (--timeout == 0) {
            printf("timeout in accel_start\n");
            return;
        }
    }

    // do rest of start
    accel_restart(addr, write);
}

void accel_send_byte(uint8_t data) {
    // send byte
    I2C1->DR = data;
    // wait for TRA, BUSY, MSL, TXE and BTF (byte transmitted)
    uint32_t timeout = 1000000;
    while ((i2c_get_sr() & 0x00070084) != 0x00070084) {
        if (--timeout == 0) {
            printf("timeout in accel_send_byte\n");
            return;
        }
    }
}

uint8_t accel_read_ack(void) {
    // enable ACK of received byte
    I2C1->CR1 |= I2C_CR1_ACK;
    // wait for BUSY, MSL and RXNE (byte received)
    uint32_t timeout = 1000000;
    while ((i2c_get_sr() & 0x00030040) != 0x00030040) {
        if (--timeout == 0) {
            printf("timeout in accel_read_ack\n");
            break;
        }
    }
    // read and return data
    uint8_t data = I2C1->DR;
    return data;
}

uint8_t accel_read_nack(void) {
    // disable ACK of received byte (to indicate end of receiving)
    I2C1->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
    // last byte should apparently also generate a stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
    // wait for BUSY, MSL and RXNE (byte received)
    uint32_t timeout = 1000000;
    while ((i2c_get_sr() & 0x00030040) != 0x00030040) {
        if (--timeout == 0) {
            printf("timeout in accel_read_nack\n");
            break;
        }
    }
    // read and return data
    uint8_t data = I2C1->DR;
    return data;
}

void accel_stop(void) {
    // send stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
}

/******************************************************************************/
/* Micro Python bindings                                                      */

int accel_buf[12];

mp_obj_t pyb_accel_read(void) {
    for (int i = 0; i <= 6; i += 3) {
        accel_buf[0 + i] = accel_buf[0 + i + 3];
        accel_buf[1 + i] = accel_buf[1 + i + 3];
        accel_buf[2 + i] = accel_buf[2 + i + 3];
    }

    accel_start(ACCEL_ADDR, 1);
    accel_send_byte(0);
    accel_restart(ACCEL_ADDR, 0);
    for (int i = 0; i <= 2; i++) {
        int v = accel_read_ack() & 0x3f;
        if (v & 0x20) {
            v |= ~0x1f;
        }
        accel_buf[9 + i] = v;
    }
    int jolt_info = accel_read_nack();

    mp_obj_t data[4];
    data[0] = mp_obj_new_int(accel_buf[0] + accel_buf[3] + accel_buf[6] + accel_buf[9]);
    data[1] = mp_obj_new_int(accel_buf[1] + accel_buf[4] + accel_buf[7] + accel_buf[10]);
    data[2] = mp_obj_new_int(accel_buf[2] + accel_buf[5] + accel_buf[8] + accel_buf[11]);
    data[3] = mp_obj_new_int(jolt_info);

    return mp_obj_new_tuple(4, data);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_accel_read_obj, pyb_accel_read);

mp_obj_t pyb_accel_read_all(void) {
    mp_obj_t data[11];
    accel_start(ACCEL_ADDR, 1);
    accel_send_byte(0);
    accel_restart(ACCEL_ADDR, 0);
    for (int i = 0; i <= 9; i++) {
        data[i] = mp_obj_new_int(accel_read_ack());
    }
    data[10] = mp_obj_new_int(accel_read_nack());

    return mp_obj_new_tuple(11, data);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_accel_read_all_obj, pyb_accel_read_all);

mp_obj_t pyb_accel_write_mode(mp_obj_t o_int, mp_obj_t o_mode) {
    accel_start(ACCEL_ADDR, 1);
    accel_send_byte(6); // start at int
    accel_send_byte(mp_obj_get_int(o_int));
    accel_send_byte(mp_obj_get_int(o_mode));
    accel_stop();
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_accel_write_mode_obj, pyb_accel_write_mode);
