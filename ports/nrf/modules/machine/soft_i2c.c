/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Afantor.lan
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "soft_i2c.h"

#define delay_us    mp_hal_delay_us 
//IIC 驱动代码	   								  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化IIC
void Soft_I2C_Init(void)
{			
    nrf_gpio_cfg_output(MICROPY_HW_I2C_SDA_PIN);       //配置管脚为输出
    nrf_gpio_cfg_output(MICROPY_HW_I2C_SCL_PIN);

	IIC_SCL_HIGH;
	IIC_SDA_HIGH;
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA_HIGH;	  	  
	IIC_SCL_HIGH;
	delay_us(4);
 	IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_LOW;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_LOW;
	IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_HIGH; 
	IIC_SDA_HIGH;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	IIC_SDA_HIGH;delay_us(1);	   
	IIC_SCL_HIGH;delay_us(1);
	SDA_IN();      //SDA设置为输入	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_LOW;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL_LOW;
	SDA_OUT();
	IIC_SDA_LOW;
	delay_us(2);
	IIC_SCL_HIGH;
	delay_us(2);
	IIC_SCL_LOW;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL_LOW;
	SDA_OUT();
	IIC_SDA_HIGH;
	delay_us(2);
	IIC_SCL_HIGH;
	delay_us(2);
	IIC_SCL_LOW;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_LOW;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {   
    	if ((txd&0x80)>>7)
    	{
    		IIC_SDA_HIGH;
    	}
    	else
    	{
    		IIC_SDA_LOW;
    	}
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL_HIGH;
		delay_us(2); 
		IIC_SCL_LOW;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL_LOW; 
        delay_us(2);
		IIC_SCL_HIGH;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


