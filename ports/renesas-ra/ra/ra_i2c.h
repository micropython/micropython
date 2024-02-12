/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#ifndef RA_RA_I2C_H_
#define RA_RA_I2C_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef RA6M5
// #define R_IIC2_BASE            0x4009F200
#ifndef R_IIC2
#define R_IIC2             ((R_IIC0_Type *)0x4009F200)
#endif
#endif

#define RA_I2C_DEF_TIMEOUT              1000    // 1000 ms
#define RA_I2C_TIMEOUT_STOP_CONDITION   100000  // counts
#define RA_I2C_TIMEOUT_BUS_BUSY         100000  // counts
#define RA_I2C_CLOCK_MAX                1000000 // counts

typedef enum
{
    RA_I2C_STATUS_Idle = 1,
    RA_I2C_STATUS_Started = 2,
    RA_I2C_STATUS_AddrWriteCompleted = 3,
    RA_I2C_STATUS_DataWriteCompleted = 4,
    RA_I2C_STATUS_DataSendCompleted = 5,
    RA_I2C_STATUS_FirstReceiveCompleted = 6,
    RA_I2C_STATUS_LastReceiveCompleted = 7,
    RA_I2C_STATUS_Stopped = 8,
} xaction_status_t;

typedef enum
{
    RA_I2C_ERROR_OK = 0,
    RA_I2C_ERROR_TMOF = 1,
    RA_I2C_ERROR_AL = 2,
    RA_I2C_ERROR_NACK = 3,
    RA_I2C_ERROR_BUSY = 4,
} xaction_error_t;

typedef struct {
    volatile uint32_t m_bytes_transferred;
    volatile uint32_t m_bytes_transfer;
    uint32_t m_bytes_total;
    bool m_fread;
    uint8_t *buf;
    void *next;
} xaction_unit_t;

typedef struct {
    xaction_unit_t *units;
    uint32_t m_num_of_units;
    uint32_t m_current;
    uint32_t m_address;
    volatile xaction_status_t m_status;
    volatile xaction_error_t m_error;
    bool m_stop;
} xaction_t;

bool ra_i2c_find_af_ch(uint32_t scl, uint32_t sda, uint8_t *ch);
void ra_i2c_irq_enable(R_IIC0_Type *i2c_inst);
void ra_i2c_irq_disable(R_IIC0_Type *i2c_inst);
void ra_i2c_priority(R_IIC0_Type *i2c_inst, uint32_t ipl);
void ra_i2c_set_baudrate(R_IIC0_Type *i2c_inst, uint32_t baudrate);
void ra_i2c_clear_IR(R_IIC0_Type *i2c_inst);
void ra_i2c_init(R_IIC0_Type *i2c_inst, uint32_t scl, uint32_t sda, uint32_t baudrate);
void ra_i2c_deinit(R_IIC0_Type *i2c_inst);
void ra_i2c_read_last_byte(R_IIC0_Type *i2c_inst);
void ra_i2c_stop_condition(R_IIC0_Type *i2c_inst);
void ra_i2c_abort(R_IIC0_Type *i2c_inst);
void ra_i2c_xaction_start(R_IIC0_Type *i2c_inst, xaction_t *xaction, bool repeated_start);
void ra_i2c_xaction_stop(void);
void ra_i2c_get_clock(uint32_t rateKhz, uint8_t *clockRate, uint8_t *clockRate2);
void ra_i2c_xunit_write_byte(R_IIC0_Type *i2c_inst, xaction_unit_t *unit);
void ra_i2c_xunit_read_byte(R_IIC0_Type *i2c_inst, xaction_unit_t *unit);
void ra_i2c_xunit_init(xaction_unit_t *unit, uint8_t *buf, uint32_t size, bool fread, void *next);
void ra_i2c_xaction_init(xaction_t *action, xaction_unit_t *units, uint32_t size, uint32_t address, bool stop);
void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);
bool ra_i2c_action_execute(R_IIC0_Type *i2c_inst, xaction_t *action, bool repeated_start, uint32_t timeout_ms);

#endif /* RA_RA_I2C_H_ */
