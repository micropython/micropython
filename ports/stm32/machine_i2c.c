/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_i2c.h"
#include "i2c.h"

#if MICROPY_HW_ENABLE_HW_I2C

STATIC const mp_obj_type_t machine_hard_i2c_type;

#if defined(STM32F4)

// F4xx specific driver for I2C hardware peripheral
// The hardware-specific I2C code below is based heavily on the code from
// V1.5.2 of the STM32 CUBE F4 HAL.  Its copyright notice is given here.
/*
* COPYRIGHT(c) 2016 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

typedef struct _machine_hard_i2c_obj_t {
    mp_obj_base_t base;
    const pyb_i2c_obj_t *pyb;
    uint32_t *timeout;
} machine_hard_i2c_obj_t;

STATIC uint32_t machine_hard_i2c_timeout[4];

STATIC const machine_hard_i2c_obj_t machine_hard_i2c_obj[] = {
    {{&machine_hard_i2c_type}, &pyb_i2c_obj[0], &machine_hard_i2c_timeout[0]},
    {{&machine_hard_i2c_type}, &pyb_i2c_obj[1], &machine_hard_i2c_timeout[1]},
    {{&machine_hard_i2c_type}, &pyb_i2c_obj[2], &machine_hard_i2c_timeout[2]},
    {{&machine_hard_i2c_type}, &pyb_i2c_obj[3], &machine_hard_i2c_timeout[3]},
};

STATIC void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u, timeout=%u)",
        self - &machine_hard_i2c_obj[0] + 1,
        i2c_get_baudrate(self->pyb->i2c),
        *self->timeout);
}

STATIC void machine_hard_i2c_init(const machine_hard_i2c_obj_t *self, uint32_t freq, uint32_t timeout) {
    *self->timeout = timeout;
    i2c_init_freq(self->pyb, freq);
}

// this function is based on STM code
STATIC bool I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c) {
    if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET) {
        /* Clear NACKF Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
        return true;
    }
    return false;
}

// this function is based on STM code
STATIC bool I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart) {
    /* Wait until flag is set */
    while ((__HAL_I2C_GET_FLAG(hi2c, Flag) ? SET : RESET) == Status) {
        if (Timeout != HAL_MAX_DELAY) {
            if ((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout)) {
                return false;
            }
        }
    }
    return true;
}

// this function is based on STM code
STATIC int I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart) {
    while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET) {
        /* Check if a STOPF is detected */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET) {
            /* Clear STOP Flag */
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
            return -MP_EBUSY;
        }

        /* Check for the Timeout */
        if ((Timeout == 0U) || ((HAL_GetTick()-Tickstart) > Timeout)) {
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

// this function is based on STM code
STATIC int send_addr_byte(I2C_HandleTypeDef *hi2c, uint8_t addr_byte, uint32_t Timeout, uint32_t Tickstart) {
    /* Generate Start */
    hi2c->Instance->CR1 |= I2C_CR1_START;

    /* Wait until SB flag is set */
    if (!I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart)) {
        return -MP_ETIMEDOUT;
    }

    /* Send slave address */
    hi2c->Instance->DR = addr_byte;

    /* Wait until ADDR flag is set */
    while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == RESET) {
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET) {
            // nack received for addr, release the bus cleanly
            hi2c->Instance->CR1 |= I2C_CR1_STOP;
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
            return -MP_ENODEV;
        }

        /* Check for the Timeout */
        if (Timeout != HAL_MAX_DELAY) {
            if ((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout)) {
                return -MP_ETIMEDOUT;
            }
        }
    }

    return 0;
}

// this function is based on STM code
int machine_hard_i2c_readfrom(mp_obj_base_t *self_in, uint16_t addr, uint8_t *dest, size_t len, bool stop) {
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t*)self_in;
    I2C_HandleTypeDef *hi2c = self->pyb->i2c;
    uint32_t Timeout = *self->timeout;

    /* Init tickstart for timeout management*/
    uint32_t tickstart = HAL_GetTick();

#if 0
    // TODO: for multi-master, here we could wait for the bus to be free
    // we'd need a flag to tell if we were in the middle of a set of transactions
    // (ie didn't send a stop bit in the last call)
    /* Wait until BUSY flag is reset */
    if (!I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart)) {
        return -MP_EBUSY;
    }
#endif

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE) {
        /* Enable I2C peripheral */
        __HAL_I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    /* Enable Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Send Slave Address */
    int ret = send_addr_byte(hi2c, I2C_7BIT_ADD_READ(addr << 1), Timeout, tickstart);
    if (ret != 0) {
        return ret;
    }

    if (len == 0U) {
        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

        /* Generate Stop */
        if (stop) {
            hi2c->Instance->CR1 |= I2C_CR1_STOP;
        }
    } else if (len == 1U) {
        /* Disable Acknowledge */
        hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

        /* Generate Stop */
        if (stop) {
            hi2c->Instance->CR1 |= I2C_CR1_STOP;
        }
    } else if (len == 2U) {
        /* Disable Acknowledge */
        hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

        /* Enable Pos */
        hi2c->Instance->CR1 |= I2C_CR1_POS;

        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    } else {
        /* Enable Acknowledge */
        hi2c->Instance->CR1 |= I2C_CR1_ACK;

        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while (len > 0U) {
        if (len <= 3U) {
            if (len == 1U) {
                /* Wait until RXNE flag is set */
                int ret = I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart);
                if (ret != 0) {
                    return ret;
                }

                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;
            } else if (len == 2U) {
                /* Wait until BTF flag is set */
                if (!I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart)) {
                  return -MP_ETIMEDOUT;
                }

                /* Generate Stop */
                if (stop) {
                    hi2c->Instance->CR1 |= I2C_CR1_STOP;
                }

                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;

                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;
            } else {
                /* Wait until BTF flag is set */
                if (!I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart)) {
                  return -MP_ETIMEDOUT;
                }

                /* Disable Acknowledge */
                hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;

                /* Wait until BTF flag is set */
                if (!I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart)) {
                    return -MP_ETIMEDOUT;
                }

                /* Generate Stop */
                if (stop) {
                    hi2c->Instance->CR1 |= I2C_CR1_STOP;
                }

                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;

                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;
            }
        } else {
            /* Wait until RXNE flag is set */
            int ret = I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart);
            if (ret != 0) {
                return ret;
            }

            /* Read data from DR */
            *dest++ = hi2c->Instance->DR;
            len--;

            if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) {
                /* Read data from DR */
                *dest++ = hi2c->Instance->DR;
                len--;
            }
        }
    }

    return 0;
}

// this function is based on STM code
int machine_hard_i2c_writeto(mp_obj_base_t *self_in, uint16_t addr, const uint8_t *src, size_t len, bool stop) {
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t*)self_in;
    I2C_HandleTypeDef *hi2c = self->pyb->i2c;
    uint32_t Timeout = *self->timeout;

    /* Init tickstart for timeout management*/
    uint32_t tickstart = HAL_GetTick();

#if 0
    // TODO: for multi-master, here we could wait for the bus to be free
    // we'd need a flag to tell if we were in the middle of a set of transactions
    // (ie didn't send a stop bit in the last call)
    /* Wait until BUSY flag is reset */
    if (!I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart)) {
        return -MP_EBUSY;
    }
#endif

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE) {
        /* Enable I2C peripheral */
        __HAL_I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    /* Send Slave Address */
    int ret = send_addr_byte(hi2c, I2C_7BIT_ADD_WRITE(addr << 1), Timeout, tickstart);
    if (ret != 0) {
        return ret;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    int num_acks = 0;

    while (len > 0U) {
        /* Wait until TXE flag is set */
        while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET) {
            /* Check if a NACK is detected */
            if (I2C_IsAcknowledgeFailed(hi2c)) {
                goto nack;
            }

            /* Check for the Timeout */
            if (Timeout != HAL_MAX_DELAY) {
                if ((Timeout == 0U) || ((HAL_GetTick()-tickstart) > Timeout)) {
                    goto timeout;
                }
            }
        }

        /* Write data to DR */
        hi2c->Instance->DR = *src++;
        len--;

        /* Wait until BTF flag is set */
        while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == RESET) {
            /* Check if a NACK is detected */
            if (I2C_IsAcknowledgeFailed(hi2c)) {
                goto nack;
            }

            /* Check for the Timeout */
            if (Timeout != HAL_MAX_DELAY) {
                if ((Timeout == 0U) || ((HAL_GetTick()-tickstart) > Timeout)) {
                    goto timeout;
                }
            }
        }
        ++num_acks;
    }
nack:

    /* Generate Stop */
    if (stop) {
        hi2c->Instance->CR1 |= I2C_CR1_STOP;
    }

    return num_acks;

timeout:
    // timeout, release the bus cleanly
    hi2c->Instance->CR1 |= I2C_CR1_STOP;
    return -MP_ETIMEDOUT;
}

#else

// No hardware I2C driver for this MCU so use the software implementation

typedef mp_machine_soft_i2c_obj_t machine_hard_i2c_obj_t;

STATIC machine_hard_i2c_obj_t machine_hard_i2c_obj[] = {
    #if defined(MICROPY_HW_I2C1_SCL)
    {{&machine_hard_i2c_type}, 1, 500, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
    #else
    {{NULL}, 0, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C2_SCL)
    {{&machine_hard_i2c_type}, 1, 500, MICROPY_HW_I2C2_SCL, MICROPY_HW_I2C2_SDA},
    #else
    {{NULL}, 0, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C3_SCL)
    {{&machine_hard_i2c_type}, 1, 500, MICROPY_HW_I2C3_SCL, MICROPY_HW_I2C3_SDA},
    #else
    {{NULL}, 0, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C4_SCL)
    {{&machine_hard_i2c_type}, 1, 500, MICROPY_HW_I2C4_SCL, MICROPY_HW_I2C4_SDA},
    #else
    {{NULL}, 0, 0, NULL, NULL},
    #endif
};

STATIC void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, scl=%q, sda=%q, freq=%u, timeout=%u)",
        self - &machine_hard_i2c_obj[0] + 1,
        self->scl->name, self->sda->name, 500000 / self->us_delay, self->us_timeout);
}

STATIC void machine_hard_i2c_init(machine_hard_i2c_obj_t *self, uint32_t freq, uint32_t timeout) {
    // set parameters
    if (freq >= 1000000) {
        // allow fastest possible bit-bang rate
        self->us_delay = 0;
    } else {
        self->us_delay = 500000 / freq;
        if (self->us_delay == 0) {
            self->us_delay = 1;
        }
    }

    self->us_timeout = timeout;

    // init pins
    mp_hal_pin_open_drain(self->scl);
    mp_hal_pin_open_drain(self->sda);
}

#define machine_hard_i2c_readfrom mp_machine_soft_i2c_readfrom
#define machine_hard_i2c_writeto mp_machine_soft_i2c_writeto

#endif

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // work out i2c bus
    int i2c_id = 0;
    if (MP_OBJ_IS_STR(args[ARG_id].u_obj)) {
        const char *port = mp_obj_str_get_str(args[ARG_id].u_obj);
        if (0) {
        #ifdef MICROPY_HW_I2C1_NAME
        } else if (strcmp(port, MICROPY_HW_I2C1_NAME) == 0) {
            i2c_id = 1;
        #endif
        #ifdef MICROPY_HW_I2C2_NAME
        } else if (strcmp(port, MICROPY_HW_I2C2_NAME) == 0) {
            i2c_id = 2;
        #endif
        #ifdef MICROPY_HW_I2C3_NAME
        } else if (strcmp(port, MICROPY_HW_I2C3_NAME) == 0) {
            i2c_id = 3;
        #endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "I2C(%s) doesn't exist", port));
        }
    } else {
        i2c_id = mp_obj_get_int(args[ARG_id].u_obj);
        if (i2c_id < 1 || i2c_id > MP_ARRAY_SIZE(machine_hard_i2c_obj)
            || machine_hard_i2c_obj[i2c_id - 1].base.type == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "I2C(%d) doesn't exist", i2c_id));
        }
    }

    // get static peripheral object
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t*)&machine_hard_i2c_obj[i2c_id - 1];

    // here we would check the scl/sda pins and configure them, but it's not implemented
    if (args[ARG_scl].u_obj != MP_OBJ_NULL || args[ARG_sda].u_obj != MP_OBJ_NULL) {
        mp_raise_ValueError("explicit choice of scl/sda is not implemented");
    }

    // initialise the I2C peripheral
    machine_hard_i2c_init(self, args[ARG_freq].u_int, args[ARG_timeout].u_int);

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_machine_i2c_p_t machine_hard_i2c_p = {
    .readfrom = machine_hard_i2c_readfrom,
    .writeto = machine_hard_i2c_writeto,
};

STATIC const mp_obj_type_t machine_hard_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_hard_i2c_print,
    .make_new = machine_hard_i2c_make_new,
    .protocol = &machine_hard_i2c_p,
    .locals_dict = (mp_obj_dict_t*)&mp_machine_soft_i2c_locals_dict,
};

#endif // MICROPY_HW_ENABLE_HW_I2C
