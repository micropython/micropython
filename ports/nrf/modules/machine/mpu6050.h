/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
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
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <stdbool.h>
#include <stdint.h>
#include "mpconfigboard.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#define MPU6050_ADDRESS         0x69
#define MPU6050_GYRO_OUT        0x43
#define MPU6050_ACC_OUT         0x3B

//#define MPU_ACCEL_OFFS_REG		0X06	//accel_offs寄存器,可读取版本号,寄存器手册未提到
//#define MPU_PROD_ID_REG			0X0C	//prod id寄存器,在寄存器手册未提到
#define MPU_XGOFFS_TC           0x00 // Bit 7 PWR_MODE, bits 6:1 XG_OFFS_TC, bit 0 OTP_BNK_VLD
#define MPU_YGOFFS_TC           0x01
#define MPU_ZGOFFS_TC           0x02
#define MPU_X_FINE_GAIN         0x03 // [7:0] fine gain
#define MPU_Y_FINE_GAIN         0x04
#define MPU_Z_FINE_GAIN         0x05
#define MPU_XA_OFFSET_H         0x06 // User-defined trim values for accelerometer
#define MPU_XA_OFFSET_L_TC      0x07
#define MPU_YA_OFFSET_H         0x08
#define MPU_YA_OFFSET_L_TC      0x09
#define MPU_ZA_OFFSET_H         0x0A
#define MPU_ZA_OFFSET_L_TC      0x0B
#define MPU_SELF_TESTX_REG		0X0D	//自检寄存器X
#define MPU_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU_SELF_TESTA_REG		0X10	//自检寄存器A
#define MPU_XG_OFFS_USRH        0x13  // User-defined trim values for gyroscope; supported in MPU-6050?
#define MPU_XG_OFFS_USRL        0x14
#define MPU_YG_OFFS_USRH        0x15
#define MPU_YG_OFFS_USRL        0x16
#define MPU_ZG_OFFS_USRH        0x17
#define MPU_ZG_OFFS_USRL        0x18
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU_CFG_REG				0X1A	//配置寄存器
#define MPU_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU_MOTION_DET_REG		0X1F	//运动检测阀值设置寄存器
#define MPU_MOT_DUR             0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MPU_ZMOT_THR            0x21  // Zero-motion detection threshold bits [7:0]
#define MPU_ZRMOT_DUR           0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms
#define MPU_FIFO_EN_REG			0X23	//FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG		0X24	//IIC主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG	0X25	//IIC从机0器件地址寄存器
#define MPU_I2CSLV0_REG			0X26	//IIC从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG	0X27	//IIC从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG	0X28	//IIC从机1器件地址寄存器
#define MPU_I2CSLV1_REG			0X29	//IIC从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG	0X2A	//IIC从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG	0X2B	//IIC从机2器件地址寄存器
#define MPU_I2CSLV2_REG			0X2C	//IIC从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG	0X2D	//IIC从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG	0X2E	//IIC从机3器件地址寄存器
#define MPU_I2CSLV3_REG			0X2F	//IIC从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG	0X30	//IIC从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG	0X31	//IIC从机4器件地址寄存器
#define MPU_I2CSLV4_REG			0X32	//IIC从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG		0X33	//IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG	0X34	//IIC从机4控制寄存器
#define MPU_I2CSLV4_DI_REG		0X35	//IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG		0X36	//IIC主机状态寄存器
#define MPU_INTBP_CFG_REG		0X37	//中断/旁路设置寄存器
#define MPU_INT_EN_REG			0X38	//中断使能寄存器
#define MPU_DMP_INT_STATUS      0x39  // Check DMP interrupt
#define MPU_INT_STA_REG			0X3A	//中断状态寄存器

#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG		0X3C	//加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG		0X3D	//加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG		0X3E	//加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG		0X3F	//加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG		0X40	//加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU_TEMP_OUTL_REG		0X42	//温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU_EXT_SENS_DATA_00    0x49
#define MPU_EXT_SENS_DATA_01    0x4A
#define MPU_EXT_SENS_DATA_02    0x4B
#define MPU_EXT_SENS_DATA_03    0x4C
#define MPU_EXT_SENS_DATA_04    0x4D
#define MPU_EXT_SENS_DATA_05    0x4E
#define MPU_EXT_SENS_DATA_06    0x4F
#define MPU_EXT_SENS_DATA_07    0x50
#define MPU_EXT_SENS_DATA_08    0x51
#define MPU_EXT_SENS_DATA_09    0x52
#define MPU_EXT_SENS_DATA_10    0x53
#define MPU_EXT_SENS_DATA_11    0x54
#define MPU_EXT_SENS_DATA_12    0x55
#define MPU_EXT_SENS_DATA_13    0x56
#define MPU_EXT_SENS_DATA_14    0x57
#define MPU_EXT_SENS_DATA_15    0x58
#define MPU_EXT_SENS_DATA_16    0x59
#define MPU_EXT_SENS_DATA_17    0x5A
#define MPU_EXT_SENS_DATA_18    0x5B
#define MPU_EXT_SENS_DATA_19    0x5C
#define MPU_EXT_SENS_DATA_20    0x5D
#define MPU_EXT_SENS_DATA_21    0x5E
#define MPU_EXT_SENS_DATA_22    0x5F
#define MPU_EXT_SENS_DATA_23    0x60
#define MPU_MOT_DETECT_STATUS   0x61

#define MPU_I2CSLV0_DO_REG		0X63	//IIC从机0数据寄存器
#define MPU_I2CSLV1_DO_REG		0X64	//IIC从机1数据寄存器
#define MPU_I2CSLV2_DO_REG		0X65	//IIC从机2数据寄存器
#define MPU_I2CSLV3_DO_REG		0X66	//IIC从机3数据寄存器

#define MPU_I2CMST_DELAY_REG	0X67	//IIC主机延时管理寄存器
#define MPU_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU_USER_CTRL_REG		0X6A	//用户控制寄存器
#define MPU_PWR_MGMT1_REG		0X6B	//电源管理寄存器1
#define MPU_PWR_MGMT2_REG		0X6C	//电源管理寄存器2
#define MPU_DMP_BANK            0x6D  // Activates a specific bank in the DMP
#define MPU_DMP_RW_PNT          0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define MPU_DMP_REG             0x6F  // Register in DMP from which to read or to which to write
#define MPU_DMP_REG_1           0x70
#define MPU_DMP_REG_2           0x71 
#define MPU_FIFO_CNTH_REG		0X72	//FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG		0X73	//FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG			0X74	//FIFO读写寄存器
#define MPU_DEVICE_ID_REG		0X75	//器件ID寄存器 .Should return 0x68
 
//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR				0X68

#define PI                      3.1415926

extern const mp_obj_type_t machine_mpu_type;

void twi_init0(void);
void twi_scan(void);

uint8_t MPU_Init(void); 								//初始化MPU6050
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);//IIC连续写
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf); //IIC连续读 
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data);				//IIC写一个字节
uint8_t MPU_Read_Byte(uint8_t reg);						//IIC读一个字节

uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr);
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr);
uint8_t MPU_Set_LPF(uint16_t lpf);
uint8_t MPU_Set_Rate(uint16_t rate);
uint8_t MPU_Set_Fifo(uint8_t sens);

float getGres(uint8_t Gscale);
float getAres(uint8_t Ascale);
void readAccelData(int16_t * destination);
void readGyroData(int16_t * destination);

void updateTime();
void LowPowerAccelOnlyMPU6050();
void initMPU6050();
void calibrateMPU6050(float * gyroBias, float * accelBias);
void MPU6050SelfTest(float * destination);
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float deltat);

short MPU_Get_Temperature(void);
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az);

extern void mpu6050_delay_ms(uint32_t ms);

#endif // __MPU6050_H__

