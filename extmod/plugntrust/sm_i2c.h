/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Arduino SA
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
#ifndef __SM_I2C_H__
#define __SM_I2C_H__

#define I2C_IDLE                (0)
#define I2C_STARTED             (1)
#define I2C_RESTARTED           (2)
#define I2C_REPEATED_START      (3)
#define DATA_ACK                (4)
#define DATA_NACK               (5)
#define I2C_BUSY                (6)
#define I2C_NO_DATA             (7)
#define I2C_NACK_ON_ADDRESS     (8)
#define I2C_NACK_ON_DATA        (9)
#define I2C_ARBITRATION_LOST    (10)
#define I2C_TIME_OUT            (11)
#define I2C_OK                  (12)
#define I2C_FAILED              (13)
#define I2C_BUS_0               (0)

typedef unsigned int i2c_error_t;
i2c_error_t axI2CInit(void **ctx, const char *dev);
void axI2CTerm(void *ctx, int mode);
i2c_error_t axI2CWrite(void *ctx, unsigned char bus, unsigned char addr, unsigned char *buf, unsigned short len);
i2c_error_t axI2CRead(void *ctx, unsigned char bus, unsigned char addr, unsigned char *buf, unsigned short len);

#endif // #ifndef __SM_I2C_H__
