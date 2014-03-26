#include <stdio.h>

#include <stm32f4xx.h>

#include "misc.h"
#include "systick.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"

typedef enum {
    PYB_I2C_1 = 0,
    PYB_I2C_2 = 1,
} pyb_i2c_t;

typedef enum {
    I2C_STATE_IDLE = 0,
    I2C_STATE_WRITE = 1,
    I2C_STATE_READ = 2,
} i2c_state_t;

// set to true if the port has already been initialized
bool i2c1_port_initialized = false;
bool i2c2_port_initialized = false;

static I2C_TypeDef * _i2c_port_addr(pyb_i2c_t i2c_port) {
    if (i2c_port == PYB_I2C_1) {
        return I2C1;
    }
    if (i2c_port == PYB_I2C_2) {
        return I2C2;
    }
    return NULL;
}

// todo - perhaps there should be some global resource management for gpio
// this function would fail if the i2c pins have already been defined for
// use by another python object
// as it is, this always returns true (unless i2c_port is invalid)
static bool _i2c_init(pyb_i2c_t i2c_port) {
    GPIO_InitTypeDef GPIO_InitStructure;

    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL)
        return false;

    if (i2c_port == PYB_I2C_1) {
        if (i2c1_port_initialized == true) {
            return true;
        }
        RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable I2C1

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


        i2c1_port_initialized = true;
    }

    if (i2c_port == PYB_I2C_2) {
        if (i2c2_port_initialized == true) {
            return true;
        }
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; // enable I2C2

        // PB10=SCL, PB11=SDA
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        // alternate functions for SCL and SDA
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

        i2c2_port_initialized = true;
    }

    // get clock speeds
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);

    // disable the I2C peripheral before we configure it
    i2c->CR1 &= ~I2C_CR1_PE;

    // program peripheral input clock
    i2c->CR2 = 4; // no interrupts; 4 MHz (hopefully!) (could go up to 42MHz)

    // configure clock control reg
    uint32_t freq = rcc_clocks.PCLK1_Frequency / (100000 << 1); // want 100kHz, this is the formula for freq
    i2c->CCR = freq; // standard mode (speed), freq calculated as above

    // configure rise time reg
    i2c->TRISE = (rcc_clocks.PCLK1_Frequency / 1000000) + 1; // formula for trise, gives maximum rise time

    // enable the I2C peripheral
    i2c->CR1 |= I2C_CR1_PE;

    return true;
}

static uint32_t _i2c_get_sr(pyb_i2c_t i2c_port) {
    // must read SR1 first, then SR2, as the read can clear some flags
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return 0;

    uint32_t sr1 = i2c->SR1;
    uint32_t sr2 = i2c->SR2;
    return (sr2 << 16) | sr1;
}

static bool _i2c_restart(pyb_i2c_t i2c_port, uint8_t addr, int write) {
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return false;

    // send start condition
    i2c->CR1 |= I2C_CR1_START;

    // wait for BUSY, MSL and SB --> Slave has acknowledged start condition
    uint32_t timeout = 1000000;
    while ((_i2c_get_sr(i2c_port) & 0x00030001) != 0x00030001) {
        if (--timeout == 0) {
            //printf("timeout in _i2c_restart\n");
            return false;
        }
    }

    if (write) {
        // send address and write bit
        i2c->DR = (addr << 1) | 0;
        // wait for BUSY, MSL, ADDR, TXE and TRA
        timeout = 1000000;
        while ((_i2c_get_sr(i2c_port) & 0x00070082) != 0x00070082) {
            if (--timeout == 0) {
                //printf("timeout in _i2c_restart write\n");
                return false;
            }
        }
    } else {
        // send address and read bit
        i2c->DR = (addr << 1) | 1;
        // wait for BUSY, MSL and ADDR flags
        timeout = 1000000;
        while ((_i2c_get_sr(i2c_port) & 0x00030002) != 0x00030002) {
            if (--timeout == 0) {
                //printf("timeout in _i2c_restart read\n");
                return false;
            }
        }
    }
    return true;
}

static bool _i2c_send_byte(pyb_i2c_t i2c_port, uint8_t data) {
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return false;

    // send byte
    i2c->DR = data;
    // wait for TRA, BUSY, MSL, TXE and BTF (byte transmitted)
    uint32_t timeout = 1000000;
    while ((_i2c_get_sr(i2c_port) & 0x00070084) != 0x00070084) {
        if (--timeout == 0) {
            //printf("timeout in _i2c_send_byte\n");
            return false;
        }
    }
    return true;
}

static uint8_t _i2c_read_ack(pyb_i2c_t i2c_port) {
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return 0;

    // enable ACK of received byte
    i2c->CR1 |= I2C_CR1_ACK;
    // wait for BUSY, MSL and RXNE (byte received)
    uint32_t timeout = 1000000;
    while ((_i2c_get_sr(i2c_port) & 0x00030040) != 0x00030040) {
        if (--timeout == 0) {
            //printf("timeout in _i2c_read_ack\n");
            break;
        }
    }
    // read and return data
    uint8_t data = i2c->DR;
    return data;
}

static uint8_t _i2c_read_nack(pyb_i2c_t i2c_port) {
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return 0;

    // disable ACK of received byte (to indicate end of receiving)
    i2c->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
    // last byte should apparently also generate a stop condition
    i2c->CR1 |= I2C_CR1_STOP;
    // wait for BUSY, MSL and RXNE (byte received)
    uint32_t timeout = 1000000;
    while ((_i2c_get_sr(i2c_port) & 0x00030040) != 0x00030040) {
        if (--timeout == 0) {
            //printf("timeout in _i2c_read_nack\n");
            break;
        }
    }
    // read and return data
    uint8_t data = i2c->DR;
    return data;
}

static bool _i2c_start(pyb_i2c_t i2c_port) {
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return false;

    // wait until I2C is not busy
    uint32_t timeout = 1000000;
    while (i2c->SR2 & I2C_SR2_BUSY) {
        if (--timeout == 0) {
            return false;
        }
    }
    return true;
}

static void _i2c_stop(pyb_i2c_t i2c_port) {
    I2C_TypeDef *i2c = _i2c_port_addr(i2c_port);
    if (i2c == NULL) return;

    // send stop condition
    i2c->CR1 |= I2C_CR1_STOP;
}

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    pyb_i2c_t i2c_port;
    int i2c_addr;
    i2c_state_t i2c_state;
} pyb_i2c_obj_t;

void i2c_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;
    print(env, "<I2C%lu addr:%lu>", (unsigned int)self->i2c_port, (unsigned int)self->i2c_addr);
}

// calls _i2c_start with write=0,1 depending on LSB of i2c_addr
mp_obj_t i2c_obj_start(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;
    if (self->i2c_state != I2C_STATE_IDLE) {
        _i2c_stop(self->i2c_port);
        self->i2c_state = I2C_STATE_IDLE;
    }
    if (_i2c_start(self->i2c_port) == true)
        return mp_const_true;
    return mp_const_false;
}

mp_obj_t i2c_obj_write(mp_obj_t self_in, mp_obj_t data_in) {
    pyb_i2c_obj_t *self = self_in;
    if (self->i2c_state != I2C_STATE_WRITE) {
        if (_i2c_restart(self->i2c_port, self->i2c_addr, 1) == false) {
            _i2c_stop(self->i2c_port);
            self->i2c_state = I2C_STATE_IDLE;
            return mp_const_false;
        }
        self->i2c_state = I2C_STATE_WRITE;
    }
    uint8_t data = mp_obj_get_int(data_in);
    if (_i2c_send_byte(self->i2c_port, data) == false)
        return mp_const_false;
    return mp_const_true;
}

mp_obj_t i2c_obj_read(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;
    if (self->i2c_state != I2C_STATE_READ) {
        if (_i2c_restart(self->i2c_port, self->i2c_addr, 0) == false) {
            _i2c_stop(self->i2c_port);
            self->i2c_state = I2C_STATE_IDLE;
            return mp_const_false;
        }
        self->i2c_state = I2C_STATE_READ;
    }
    uint8_t data = _i2c_read_ack(self->i2c_port);
    return mp_obj_new_int(data);
}

mp_obj_t i2c_obj_readAndStop(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;
    if (self->i2c_state != I2C_STATE_READ) {
        if (_i2c_restart(self->i2c_port, self->i2c_addr, 0) == false) {
            _i2c_stop(self->i2c_port);
            self->i2c_state = I2C_STATE_IDLE;
            return mp_const_false;
        }
    }
    uint8_t data = _i2c_read_nack(self->i2c_port);
    self->i2c_state = I2C_STATE_IDLE;
    return mp_obj_new_int(data);
}

mp_obj_t i2c_obj_stop(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;
    _i2c_stop(self->i2c_port);
    self->i2c_state = I2C_STATE_IDLE;
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(i2c_obj_start_obj, i2c_obj_start);
static MP_DEFINE_CONST_FUN_OBJ_2(i2c_obj_write_obj, i2c_obj_write);
static MP_DEFINE_CONST_FUN_OBJ_1(i2c_obj_read_obj, i2c_obj_read);
static MP_DEFINE_CONST_FUN_OBJ_1(i2c_obj_readAndStop_obj, i2c_obj_readAndStop);
static MP_DEFINE_CONST_FUN_OBJ_1(i2c_obj_stop_obj, i2c_obj_stop);

STATIC const mp_map_elem_t i2c_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_start), (mp_obj_t)&i2c_obj_start_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&i2c_obj_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&i2c_obj_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readAndStop), (mp_obj_t)&i2c_obj_readAndStop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop), (mp_obj_t)&i2c_obj_stop_obj },
};

STATIC MP_DEFINE_CONST_DICT(i2c_locals_dict, i2c_locals_dict_table);

static const mp_obj_type_t i2c_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = i2c_obj_print,
    .locals_dict = (mp_obj_t)&i2c_locals_dict,
};

// create the I2C object
// currently support either I2C1 (i2c_id = 0) or I2C2 (i2c_id = 1)

mp_obj_t pyb_I2C(mp_obj_t i2c_id, mp_obj_t i2c_addr) {
    pyb_i2c_t i2c_port;
    switch(mp_obj_get_int(i2c_id)) {
        case 0: i2c_port = PYB_I2C_1; break;
        case 1: i2c_port = PYB_I2C_2; break;
        default: return mp_const_none;
    }
    if (_i2c_init(i2c_port) == false) {
        return mp_const_none;
    }
    pyb_i2c_obj_t *o = m_new_obj(pyb_i2c_obj_t);
    o->base.type = &i2c_obj_type;
    o->i2c_port = i2c_port;
    o->i2c_addr = mp_obj_get_int(i2c_addr);
    o->i2c_state = I2C_STATE_IDLE;
    return o;
}
