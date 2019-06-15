/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Afantor
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

#ifndef __SOFT_I2C_H__
#define __SOFT_I2C_H__

#include "mphalport.h"
#include "mpconfigboard.h"

#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

//IO方向设置
#define SDA_IN()  nrf_gpio_cfg_input(MICROPY_HW_I2C_SDA_PIN,NRF_GPIO_PIN_PULLUP) //SDA输入模式
#define SDA_OUT() nrf_gpio_cfg_output(MICROPY_HW_I2C_SDA_PIN) //SDA输出模式
//IO操作函数   
#define IIC_SCL_HIGH    nrf_gpio_pin_set(MICROPY_HW_I2C_SCL_PIN)      //SCL_HIGH
#define IIC_SCL_LOW     nrf_gpio_pin_clear(MICROPY_HW_I2C_SCL_PIN)      //SCL_LOW
#define IIC_SDA_HIGH    nrf_gpio_pin_set(MICROPY_HW_I2C_SDA_PIN)      //SDA_HIGH
#define IIC_SDA_LOW     nrf_gpio_pin_clear(MICROPY_HW_I2C_SDA_PIN)      //SDA_LOW
#define READ_SDA        nrf_gpio_pin_read(MICROPY_HW_I2C_SDA_PIN)       //读输入SDA 

//IIC所有操作函数
void Soft_I2C_Init(void);         //初始化IIC的IO口         
void IIC_Start(void);             //发送IIC开始信号
void IIC_Stop(void);              //发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);       //IIC发送一个字节
uint8_t IIC_Read_Byte(uint8_t ack);    //IIC读取一个字节
uint8_t IIC_Wait_Ack(void);            //IIC等待ACK信号
void IIC_Ack(void);               //IIC发送ACK信号
void IIC_NAck(void);              //IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr); 

#endif // __SOFT_I2C_H__
