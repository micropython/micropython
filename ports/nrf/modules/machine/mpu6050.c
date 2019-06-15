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
 * THE SOFTWAREI.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "nrfx_twi.h"

#include "mphalport.h"
#include "mpconfigboard.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#include "py/obj.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"

#include "soft_i2c.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"

#if MICROPY_PY_MACHINE_MPU6050

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0


 /* Number of possible TWI addresses. */
 #define TWI_ADDRESSES      127

/* TWI instance. */
static const nrfx_twi_t p_twi = NRFX_TWI_INSTANCE(TWI_INSTANCE_ID);

#define TWI_READ_BIT                 (0x01)        //!< If this bit is set in the address field, transfer direction is from slave to master.
/*lint ++flb "Enter library region" */

#define ADDRESS_WHO_AM_I          (0x75U) // !< WHO_AM_I register identifies the device. Expected value is 0x68.
#define ADDRESS_SIGNAL_PATH_RESET (0x68U) // !<

enum Ascale {
      AFS_2G = 0,
      AFS_4G,
      AFS_8G,
      AFS_16G
    };
    
enum Gscale {
      GFS_250DPS = 0,
      GFS_500DPS,
      GFS_1000DPS,
      GFS_2000DPS
    };
// Specify sensor full scale
uint8_t Gscale = GFS_2000DPS;
uint8_t Ascale = AFS_2G;

float pitch=0, yaw=0, roll=0;
float pre_pitch, pre_yaw, pre_roll;
float temperature;   // Stores the real internal chip temperature in Celsius
int16_t tempCount;   // Temperature raw count output
uint32_t delt_t = 0; // Used to control display output rate

uint32_t count = 0, sumCount = 0; // used to control display output rate
float deltat = 0.0f, sum = 0.0f;  // integration interval for both filter schemes
uint32_t lastUpdate = 0, firstUpdate = 0; // used to calculate integration interval
uint32_t Now = 0;        // used to calculate integration interval

static short gyroOffset[3] = {0,0,0};
static short accelOffset[3] = {0,0,0};

int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
// Scale resolutions per LSB for the sensors
float aRes, gRes;
// Variables to hold latest sensor data values
float ax, ay, az, gx, gy, gz;
// Bias corrections for gyro and accelerometer
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};
float SelfTest[6];
// Stores the 16-bit signed accelerometer sensor output
int16_t accelCount[3];   
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};            // vector to hold quaternion
// parameters for 6 DoF sensor fusion calculations
float GyroMeasError = PI * (40.0f / 180.0f);     // gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
float beta = (float)sqrt(3.0f / 4.0f) * (float)(PI * (40.0f / 180.0f));  // compute beta
float GyroMeasDrift = PI * (2.0f / 180.0f);      // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
float zeta = (float)sqrt(3.0f / 4.0f) * (float)(PI * (2.0f / 180.0f));  // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value


typedef struct _machine_mpu_obj_t {
    mp_obj_base_t base;
} machine_mpu_obj_t;


/**
 * @brief TWI initialization.
 */
void twi_init0 (void)
{
    const nrfx_twi_config_t twi_config = {
       .scl                = MICROPY_HW_I2C_SCL_PIN,
       .sda                = MICROPY_HW_I2C_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       // .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .hold_bus_uninit    = false
    };

    nrfx_twi_init(&p_twi, &twi_config, NULL, NULL);

    nrfx_twi_enable(&p_twi);
}

/**
 * @brief TWI Scan.
 */
void twi_scan (void)
{
    uint8_t address;
    uint8_t sample_data;
    bool detected_device = false;

    nrfx_err_t err_code;

    printf("TWI scanner started.\n");
    twi_init0();

    for (address = 1; address <= TWI_ADDRESSES; address++)
    {
        err_code = nrfx_twi_rx(&p_twi, address, &sample_data, sizeof(sample_data));
        if (err_code == NRFX_SUCCESS)
        {
            detected_device = true;
            printf("TWI device detected at address 0x%x.\n", address);
        }
    }

    if (!detected_device)
    {
        printf("No device was found.\n");
    }
    nrfx_twi_disable(&p_twi);
}

void mpu6050_delay_ms(uint32_t ms)
{
    mp_hal_delay_ms(ms);
}
//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    uint8_t i; 
    IIC_Start(); 
    IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令 
    if(IIC_Wait_Ack())  //等待应答
    {
        IIC_Stop();      
        return 1;       
    }
    IIC_Send_Byte(reg); //写寄存器地址
    IIC_Wait_Ack();     //等待应答
    for(i=0;i<len;i++)
    {
        IIC_Send_Byte(buf[i]);  //发送数据
        if(IIC_Wait_Ack())      //等待ACK
        {
            IIC_Stop();  
            return 1;        
        }       
    }    
    IIC_Stop();  
    return 0;   
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
    IIC_Start(); 
    IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令 
    if(IIC_Wait_Ack())  //等待应答
    {
        IIC_Stop();      
        return 1;       
    }
    IIC_Send_Byte(reg); //写寄存器地址
    IIC_Wait_Ack();     //等待应答
    IIC_Start();
    IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令 
    IIC_Wait_Ack();     //等待应答 
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
        else *buf=IIC_Read_Byte(1);     //读数据,发送ACK  
        len--;
        buf++; 
    }    
    IIC_Stop(); //产生一个停止条件 
    return 0;   
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data)                
{ 
    IIC_Start(); 
    IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令 
    if(IIC_Wait_Ack())  //等待应答
    {
        IIC_Stop();      
        return 1;       
    }
    IIC_Send_Byte(reg); //写寄存器地址
    IIC_Wait_Ack();     //等待应答 
    IIC_Send_Byte(data);//发送数据
    if(IIC_Wait_Ack())  //等待ACK
    {
        IIC_Stop();  
        return 1;        
    }        
    IIC_Stop();  
    return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
uint8_t MPU_Read_Byte(uint8_t reg)
{
    uint8_t res;
    IIC_Start(); 
    IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令 
    IIC_Wait_Ack();     //等待应答 
    IIC_Send_Byte(reg); //写寄存器地址
    IIC_Wait_Ack();     //等待应答
    IIC_Start();
    IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令 
    IIC_Wait_Ack();     //等待应答 
    res = IIC_Read_Byte(0);//读取数据,发送nACK 
    IIC_Stop();         //产生一个停止条件 
    return res;     
}

//初始化MPU6050
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Init(void)
{ 
    uint8_t res;
    Soft_I2C_Init();//初始化IIC总线
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80); //复位MPU6050
    mp_hal_delay_ms(100);
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00); //唤醒MPU6050 
    MPU_Set_Gyro_Fsr(3);                    //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                   //加速度传感器,±2g
    MPU_Set_Rate(50);                       //设置采样率50Hz
    MPU_Write_Byte(MPU_INT_EN_REG,0X00);    //关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG,0X00); //I2C主模式关闭
    MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);   //关闭FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80); //INT引脚低电平有效
    res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if(res==MPU_ADDR)//器件ID正确
    {
        MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01); //设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00); //加速度与陀螺仪都工作
        MPU_Set_Rate(50);                       //设置采样率为50Hz
    }
    else
    {
        return 1;
    }
    return 0;
}
//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
    uint8_t data=0;
    if(lpf>=188)data=1;
    else if(lpf>=98)data=2;
    else if(lpf>=42)data=3;
    else if(lpf>=20)data=4;
    else if(lpf>=10)data=5;
    else data=6; 
    return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Rate(uint16_t rate)
{
    uint8_t data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);  //设置数字低通滤波器
    return MPU_Set_LPF(rate/2); //自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    uint8_t buf[2]; 
    short raw;
    float temp;
    double temp0;
    MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw = ((uint16_t)buf[0]<<8)|buf[1];
    temp0 = ((double)raw)/340;
    temp = 36.53 + (float)temp0;  
    return temp*100;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],res;  
    res = MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
    if(res==0)
    {
        *gx=((uint16_t)buf[0]<<8)|buf[1];  
        *gy=((uint16_t)buf[2]<<8)|buf[3];  
        *gz=((uint16_t)buf[4]<<8)|buf[5];
    }   
    return res;;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
    res = MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
    if(res==0)
    {
        *ax=((uint16_t)buf[0]<<8)|buf[1];  
        *ay=((uint16_t)buf[2]<<8)|buf[3];  
        *az=((uint16_t)buf[4]<<8)|buf[5];
    }   
    return res;
}

//==============================================================================
//====== Set of useful function to access acceleration. gyroscope, 
//====== and temperature data
//==============================================================================
float getGres(uint8_t Gscale) 
{
  switch (Gscale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case GFS_250DPS:
      return 250.0 / 32768.0;
      break;
    case GFS_500DPS:
      return 500.0 / 32768.0;
      break;
    case GFS_1000DPS:
      return 1000.0 / 32768.0;
      break;
    case GFS_2000DPS:
      return 2000.0 / 32768.0;
      break;
  }
  return 0;
}

float getAres(uint8_t Ascale) 
{
  switch (Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
      return 2.0 / 32768.0;
      break;
    case AFS_4G:
      return 4.0 / 32768.0;
      break;
    case AFS_8G:
      return 8.0 / 32768.0;
      break;
    case AFS_16G:
      return 16.0 / 32768.0;
      break;
  }
  return 0;
}

// Configure the motion detection control for low power accelerometer mode
void LowPowerAccelOnlyMPU6050()
{

  // The sensor has a high-pass filter necessary to invoke to allow the sensor motion detection algorithms work properly
  // Motion detection occurs on free-fall (acceleration below a threshold for some time for all axes), motion (acceleration
  // above a threshold for some time on at least one axis), and zero-motion toggle (acceleration on each axis less than a
  // threshold for some time sets this flag, motion above the threshold turns it off). The high-pass filter takes gravity out
  // consideration for these threshold evaluations; otherwise, the flags would be set all the time!

  uint8_t c = MPU_Read_Byte(MPU_PWR_MGMT1_REG);
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, c & ~0x30); // Clear sleep and cycle bits [5:6]
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, c |  0x30); // Set sleep and cycle bits [5:6] to zero to make sure accelerometer is running

  c = MPU_Read_Byte(MPU_PWR_MGMT2_REG);
  MPU_Write_Byte(MPU_PWR_MGMT2_REG, c & ~0x38); // Clear standby XA, YA, and ZA bits [3:5]
  MPU_Write_Byte(MPU_PWR_MGMT2_REG, c |  0x00); // Set XA, YA, and ZA bits [3:5] to zero to make sure accelerometer is running

  c = MPU_Read_Byte(MPU_ACCEL_CFG_REG);
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, c & ~0x07); // Clear high-pass filter bits [2:0]
  // Set high-pass filter to 0) reset (disable), 1) 5 Hz, 2) 2.5 Hz, 3) 1.25 Hz, 4) 0.63 Hz, or 7) Hold
  MPU_Write_Byte(MPU_ACCEL_CFG_REG,  c | 0x00);  // Set ACCEL_HPF to 0; reset mode disbaling high-pass filter

  c = MPU_Read_Byte(MPU_CFG_REG);
  MPU_Write_Byte(MPU_CFG_REG, c & ~0x07); // Clear low-pass filter bits [2:0]
  MPU_Write_Byte(MPU_CFG_REG, c |  0x00);  // Set DLPD_CFG to 0; 260 Hz bandwidth, 1 kHz rate

  c = MPU_Read_Byte(MPU_INT_EN_REG);
  MPU_Write_Byte(MPU_INT_EN_REG, c & 0x00);  // Clear all interrupts
  MPU_Write_Byte(MPU_INT_EN_REG, 0x40);  // Enable motion threshold (bits 5) interrupt only

  // Motion detection interrupt requires the absolute value of any axis to lie above the detection threshold
  // for at least the counter duration
  MPU_Write_Byte(MPU_MOTION_DET_REG, 0x80); // Set motion detection to 0.256 g; LSB = 2 mg
  MPU_Write_Byte(MPU_MOT_DUR, 0x01); // Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate

  mp_hal_delay_ms (100);  // Add delay for accumulation of samples

  c = MPU_Read_Byte(MPU_ACCEL_CFG_REG);
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, c & ~0x07); // Clear high-pass filter bits [2:0]
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, c |  0x07);  // Set ACCEL_HPF to 7; hold the initial accleration value as a referance

  c = MPU_Read_Byte(MPU_PWR_MGMT2_REG);
  MPU_Write_Byte(MPU_PWR_MGMT2_REG, c & ~0xC7); // Clear standby XA, YA, and ZA bits [3:5] and LP_WAKE_CTRL bits [6:7]
  MPU_Write_Byte(MPU_PWR_MGMT2_REG, c |  0x47); // Set wakeup frequency to 5 Hz, and disable XG, YG, and ZG gyros (bits [0:2])

  c = MPU_Read_Byte(MPU_PWR_MGMT1_REG);
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, c & ~0x20); // Clear sleep and cycle bit 5
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, c |  0x20); // Set cycle bit 5 to begin low power accelerometer motion interrupts

}


void initMPU6050()
{
  // wake up device-don't need this here if using calibration function below
  //  MPU_Write_Byte(MPU6050_ADDRESS, MPU_PWR_MGMT1_REG, 0x00); // Clear sleep mode bit (6), enable all sensors
  //  delay(100); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt
  // reset device, reset all registers, clear gyro and accelerometer bias registers
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  mp_hal_delay_ms(100);
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x00);
  // get stable time source
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

  // Configure Gyro and Accelerometer
  // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
  // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
  // Maximum delay time is 4.9 ms corresponding to just over 200 Hz sample rate
  MPU_Write_Byte(MPU_CFG_REG, 0x03);

  // Set sample rate = gyroscope output rate/(1 + MPU_SAMPLE_RATE_REG)
  MPU_Write_Byte(MPU_SAMPLE_RATE_REG, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above

  // Set gyroscope full scale range
  // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
  uint8_t c =  MPU_Read_Byte(MPU_GYRO_CFG_REG);
  MPU_Write_Byte(MPU_GYRO_CFG_REG, c & ~0xE0); // Clear self-test bits [7:5]
  MPU_Write_Byte(MPU_GYRO_CFG_REG, c & ~0x18); // Clear AFS bits [4:3]
  MPU_Write_Byte(MPU_GYRO_CFG_REG, c | Gscale << 3); // Set full scale range for the gyro

  // Set accelerometer configuration
  c =  MPU_Read_Byte(MPU_ACCEL_CFG_REG);
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, c & ~0xE0); // Clear self-test bits [7:5]
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, c & ~0x18); // Clear AFS bits [4:3]
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, c | Ascale << 3); // Set full scale range for the accelerometer

  // Configure Interrupts and Bypass Enable
  // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips
  // can join the I2C bus and all can be controlled by the Arduino as master
  MPU_Write_Byte(MPU_INTBP_CFG_REG, 0x22);
  MPU_Write_Byte(MPU_INT_EN_REG, 0x01);  // Enable data ready (bit 0) interrupt
  mp_hal_delay_ms(50);
}

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void calibrateMPU6050(float * dest1, float * dest2)
{
  uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t ii, packet_count, fifo_count;
  int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

  // reset device, reset all registers, clear gyro and accelerometer bias registers
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  mp_hal_delay_ms(100);

  // get stable time source
  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x01);
  MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0x00);
  mp_hal_delay_ms(200);

  // Configure device for bias calculation
  MPU_Write_Byte(MPU_INT_EN_REG, 0x00);   // Disable all interrupts
  MPU_Write_Byte(MPU_FIFO_EN_REG, 0x00);      // Disable FIFO
  MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x00);   // Turn on internal clock source
  MPU_Write_Byte(MPU_I2CMST_CTRL_REG, 0x00); // Disable I2C master
  MPU_Write_Byte(MPU_USER_CTRL_REG, 0x00);    // Disable FIFO and I2C master modes
  MPU_Write_Byte(MPU_USER_CTRL_REG, 0x0C);    // Reset FIFO and DMP
  mp_hal_delay_ms(15);

  // Configure MPU6050 gyro and accelerometer for bias calculation
  MPU_Write_Byte(MPU_CFG_REG, 0x01);      // Set low-pass filter to 188 Hz
  MPU_Write_Byte(MPU_SAMPLE_RATE_REG, 0x00);  // Set sample rate to 1 kHz
  MPU_Write_Byte(MPU_GYRO_CFG_REG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

  uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

  // Configure FIFO to capture accelerometer and gyro data for bias calculation
  MPU_Write_Byte(MPU_USER_CTRL_REG, 0x40);   // Enable FIFO
  MPU_Write_Byte(MPU_FIFO_EN_REG, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 1024 bytes in MPU-6050)
  mp_hal_delay_ms(80); // accumulate 80 samples in 80 milliseconds = 960 bytes

  // At end of sample accumulation, turn off FIFO sensor read
  MPU_Write_Byte(MPU_FIFO_EN_REG, 0x00);        // Disable gyro and accelerometer sensors for FIFO
  MPU_Read_Len(MPU_ADDR,MPU_FIFO_CNTH_REG, 2, &data[0]); // read FIFO sample count
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  packet_count = fifo_count / 12; // How many sets of full gyro and accelerometer data for averaging

  for (ii = 0; ii < packet_count; ii++) {
    int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
    MPU_Read_Len(MPU_ADDR,MPU_FIFO_RW_REG, 12, &data[0]); // read data for averaging
    accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
    accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
    gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
    gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

    accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];

  }
  accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
  accel_bias[1] /= (int32_t) packet_count;
  accel_bias[2] /= (int32_t) packet_count;
  gyro_bias[0]  /= (int32_t) packet_count;
  gyro_bias[1]  /= (int32_t) packet_count;
  gyro_bias[2]  /= (int32_t) packet_count;

  if (accel_bias[2] > 0L) {
    accel_bias[2] -= (int32_t) accelsensitivity; // Remove gravity from the z-axis accelerometer bias calculation
  }
  else {
    accel_bias[2] += (int32_t) accelsensitivity;
  }

  // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  data[0] = (-gyro_bias[0] / 4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  data[1] = (-gyro_bias[0] / 4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
  data[2] = (-gyro_bias[1] / 4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1] / 4)       & 0xFF;
  data[4] = (-gyro_bias[2] / 4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2] / 4)       & 0xFF;

  // Push gyro biases to hardware registers
  MPU_Write_Byte(MPU_XG_OFFS_USRH, data[0]);// might not be supported in MPU6050
  MPU_Write_Byte(MPU_XG_OFFS_USRL, data[1]);
  MPU_Write_Byte(MPU_YG_OFFS_USRH, data[2]);
  MPU_Write_Byte(MPU_YG_OFFS_USRL, data[3]);
  MPU_Write_Byte(MPU_ZG_OFFS_USRH, data[4]);
  MPU_Write_Byte(MPU_ZG_OFFS_USRL, data[5]);

  dest1[0] = (float) gyro_bias[0] / (float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
  dest1[1] = (float) gyro_bias[1] / (float) gyrosensitivity;
  dest1[2] = (float) gyro_bias[2] / (float) gyrosensitivity;

  // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
  // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
  // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
  // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
  // the accelerometer biases calculated above must be divided by 8.

  int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
  MPU_Read_Len(MPU_ADDR,MPU_XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
  accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  MPU_Read_Len(MPU_ADDR,MPU_YA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  MPU_Read_Len(MPU_ADDR,MPU_ZA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

  uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
  uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

  for (ii = 0; ii < 3; ii++) {
    if (accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
  }

  // Construct total accelerometer bias, including calculated average accelerometer bias from above
  accel_bias_reg[0] -= (accel_bias[0] / 8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
  accel_bias_reg[1] -= (accel_bias[1] / 8);
  accel_bias_reg[2] -= (accel_bias[2] / 8);

  data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  data[1] = (accel_bias_reg[0])      & 0xFF;
  data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  data[3] = (accel_bias_reg[1])      & 0xFF;
  data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  data[5] = (accel_bias_reg[2])      & 0xFF;
  data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

  // Push accelerometer biases to hardware registers
  MPU_Write_Byte(MPU_XA_OFFSET_H, data[0]); // might not be supported in MPU6050
  MPU_Write_Byte(MPU_XA_OFFSET_L_TC, data[1]);
  MPU_Write_Byte(MPU_YA_OFFSET_H, data[2]);
  MPU_Write_Byte(MPU_YA_OFFSET_L_TC, data[3]);
  MPU_Write_Byte(MPU_ZA_OFFSET_H, data[4]);
  MPU_Write_Byte(MPU_ZA_OFFSET_L_TC, data[5]);

  // Output scaled accelerometer biases for manual subtraction in the main program
  dest2[0] = (float)accel_bias[0] / (float)accelsensitivity;
  dest2[1] = (float)accel_bias[1] / (float)accelsensitivity;
  dest2[2] = (float)accel_bias[2] / (float)accelsensitivity;
}


// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU6050SelfTest(float * destination) // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
  uint8_t rawData[4];
  uint8_t selfTest[6];
  float factoryTrim[6];

  // Configure the accelerometer for self-test
  MPU_Write_Byte(MPU_ACCEL_CFG_REG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
  MPU_Write_Byte(MPU_GYRO_CFG_REG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
  mp_hal_delay_ms(250);  // Delay a while to let the device execute the self-test
  rawData[0] = MPU_Read_Byte(MPU_SELF_TESTX_REG); // X-axis self-test results
  rawData[1] = MPU_Read_Byte(MPU_SELF_TESTY_REG); // Y-axis self-test results
  rawData[2] = MPU_Read_Byte(MPU_SELF_TESTZ_REG); // Z-axis self-test results
  rawData[3] = MPU_Read_Byte(MPU_SELF_TESTA_REG); // Mixed-axis self-test results
  // Extract the acceleration test results first
  selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // XA_TEST result is a five-bit unsigned integer
  selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 2 ; // YA_TEST result is a five-bit unsigned integer
  selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) ; // ZA_TEST result is a five-bit unsigned integer
  // Extract the gyration test results first
  selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
  selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
  selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer
  // Process results to allow final comparison with factory set values
  factoryTrim[0] = (4096.0 * 0.34) * (float)(powf( (0.92 / 0.34) , (((float)selfTest[0] - 1.0) / 30.0))); // FT[Xa] factory trim calculation
  factoryTrim[1] = (4096.0 * 0.34) * (float)(powf( (0.92 / 0.34) , (((float)selfTest[1] - 1.0) / 30.0))); // FT[Ya] factory trim calculation
  factoryTrim[2] = (4096.0 * 0.34) * (float)(powf( (0.92 / 0.34) , (((float)selfTest[2] - 1.0) / 30.0))); // FT[Za] factory trim calculation
  factoryTrim[3] =  ( 25.0 * 131.0) * (float)(powf( 1.046 , ((float)selfTest[3] - 1.0) ));         // FT[Xg] factory trim calculation
  factoryTrim[4] =  (-25.0 * 131.0) * (float)(powf( 1.046 , ((float)selfTest[4] - 1.0) ));         // FT[Yg] factory trim calculation
  factoryTrim[5] =  ( 25.0 * 131.0) * (float)(powf( 1.046 , ((float)selfTest[5] - 1.0) ));         // FT[Zg] factory trim calculation

  // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
  // To get to percent, must multiply by 100 and subtract result from 100
  for (int i = 0; i < 6; i++) {
    destination[i] = 100.0 + 100.0 * ((float)selfTest[i] - factoryTrim[i]) / factoryTrim[i]; // Report percent differences
  }
}

// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration and rotation rate to produce a quaternion-based estimate of relative
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!

/*
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float deltat)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];         // short name local variable for readability
    float norm;                                               // vector norm
    float f1, f2, f3;                                         // objetive funcyion elements
    float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; // objective function Jacobian elements
    float qDot1, qDot2, qDot3, qDot4;
    float hatDot1, hatDot2, hatDot3, hatDot4;
    float gerrx, gerry, gerrz, gbiasx, gbiasy, gbiasz;        // gyro bias error

    // Auxiliary variables to avoid repeated arithmetic
    float _halfq1 = 0.5f * q1;
    float _halfq2 = 0.5f * q2;
    float _halfq3 = 0.5f * q3;
    float _halfq4 = 0.5f * q4;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;
            
    // Compute the objective function and Jacobian
    f1 = _2q2 * q4 - _2q1 * q3 - ax;
    f2 = _2q1 * q2 + _2q3 * q4 - ay;
    f3 = 1.0f - _2q2 * q2 - _2q3 * q3 - az;
    J_11or24 = _2q3;
    J_12or23 = _2q4;
    J_13or22 = _2q1;
    J_14or21 = _2q2;
    J_32 = 2.0f * J_14or21;
    J_33 = 2.0f * J_11or24;
          
    // Compute the gradient (matrix multiplication)
    hatDot1 = J_14or21 * f2 - J_11or24 * f1;
    hatDot2 = J_12or23 * f1 + J_13or22 * f2 - J_32 * f3;
    hatDot3 = J_12or23 * f2 - J_33 *f3 - J_13or22 * f1;
    hatDot4 = J_14or21 * f1 + J_11or24 * f2;
            
    // Normalize the gradient
    norm = sqrt(hatDot1 * hatDot1 + hatDot2 * hatDot2 + hatDot3 * hatDot3 + hatDot4 * hatDot4);
    hatDot1 /= norm;
    hatDot2 /= norm;
    hatDot3 /= norm;
    hatDot4 /= norm;
            
    // Compute estimated gyroscope biases
    gerrx = _2q1 * hatDot2 - _2q2 * hatDot1 - _2q3 * hatDot4 + _2q4 * hatDot3;
    gerry = _2q1 * hatDot3 + _2q2 * hatDot4 - _2q3 * hatDot1 - _2q4 * hatDot2;
    gerrz = _2q1 * hatDot4 - _2q2 * hatDot3 + _2q3 * hatDot2 - _2q4 * hatDot1;
            
    // Compute and remove gyroscope biases
    gbiasx += gerrx * deltat * zeta;
    gbiasy += gerry * deltat * zeta;
    gbiasz += gerrz * deltat * zeta;
    gx -= gbiasx;
    gy -= gbiasy;
    gz -= gbiasz;
            
    // Compute the quaternion derivative
    qDot1 = -_halfq2 * gx - _halfq3 * gy - _halfq4 * gz;
    qDot2 =  _halfq1 * gx + _halfq3 * gz - _halfq4 * gy;
    qDot3 =  _halfq1 * gy - _halfq2 * gz + _halfq4 * gx;
    qDot4 =  _halfq1 * gz + _halfq2 * gy - _halfq3 * gx;

    // Compute then integrate estimated quaternion derivative
    q1 += (qDot1 -(beta * hatDot1)) * deltat;
    q2 += (qDot2 -(beta * hatDot2)) * deltat;
    q3 += (qDot3 -(beta * hatDot3)) * deltat;
    q4 += (qDot4 -(beta * hatDot4)) * deltat;

    // Normalize the quaternion
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;
}
*/

/// \method __str__()
/// Return a string describing the mpu object.
STATIC void machine_mpu_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_mpu_obj_t *self = o;

    (void)self;

    mp_printf(print, "IMU MPU6050");
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_mpu_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    machine_mpu_obj_t *self = m_new_obj(machine_mpu_obj_t);
    
    self->base.type = &machine_mpu_type;

    return MP_OBJ_FROM_PTR(self);
}

/// \method scan()
/// scan i2c device.
STATIC mp_obj_t machine_mpu_scan(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    (void)self;
    twi_init0();
    twi_scan();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_scan_obj, machine_mpu_scan);

/// \method init()
/// mpu6050 init.
STATIC mp_obj_t machine_mpu_init(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    (void)self;
    if(MPU_Init() != 0){
        printf("MPU6050 init error.\n");
    }
    mp_hal_delay_ms(10);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_init_obj, machine_mpu_init);

/// \method initMPU()
/// mpu6050 init.
STATIC mp_obj_t machine_mpu_initMPU(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    (void)self;
    
    initMPU6050();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_initMPU_obj, machine_mpu_initMPU);

/// \method dmp_init()
/// mpu6050 dmp init.
STATIC mp_obj_t machine_mpu_DMPinit(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    (void)self;
    if(MPU_Init() != 0){
        printf("MPU6050 ID error.\n");
    }
    mp_hal_delay_ms(20);
    while(mpu_dmp_init())
    {
        printf("MPU6050 DMP init error.\n");
        mp_hal_delay_ms(300);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_DMPinit_obj, machine_mpu_DMPinit);

/// \method getTemp()
/// mpu6050 get Soc Temperature.
STATIC mp_obj_t machine_mpu_getTemp(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    float Temp;
    (void)self;

    Temp = MPU_Get_Temperature();

    return mp_obj_new_float(Temp/100);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getTemp_obj, machine_mpu_getTemp);

/// \method getAccel()
/// mpu6050 get Accelerometer.
STATIC mp_obj_t machine_mpu_getAccel(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short aacx,aacy,aacz;       //加速度传感器原始数据
    mp_obj_t tuple[3];
    (void)self;

    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据

    tuple[0] = mp_obj_new_int(aacx);
    tuple[1] = mp_obj_new_int(aacy);
    tuple[2] = mp_obj_new_int(aacz);

    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getAccel_obj, machine_mpu_getAccel);

/// \method getGyros()
/// mpu6050 get Gyroscope.
STATIC mp_obj_t machine_mpu_getGyros(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short gyrox,gyroy,gyroz;    //陀螺仪原始数据
    mp_obj_t tuple[3];
    (void)self;

    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据

    tuple[0] = mp_obj_new_int(gyrox);
    tuple[1] = mp_obj_new_int(gyroy);
    tuple[2] = mp_obj_new_int(gyroz);

    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getGyros_obj, machine_mpu_getGyros);

/// \method setAccelOffset()
/// set Accel data Offset.
STATIC mp_obj_t machine_mpu_setAccelOffset(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t z = mp_obj_get_int(args[3]);

    accelOffset[0] = x;
    accelOffset[1] = y;
    accelOffset[2] = z;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_mpu_setAccelOffset_obj, 4, 4, machine_mpu_setAccelOffset);

/// \method setGyrosOffset()
/// set Gyros data Offset.
STATIC mp_obj_t machine_mpu_setGyrosOffset(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t z = mp_obj_get_int(args[3]);

    gyroOffset[0] = x;
    gyroOffset[1] = y;
    gyroOffset[2] = z;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_mpu_setGyrosOffset_obj, 4, 4, machine_mpu_setGyrosOffset);

/// \method getDMPdata()
/// mpu6050 get DMP data.
STATIC mp_obj_t machine_mpu_getDMPdata(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;

    uint8_t r_dmp = 0;
    mp_obj_t tuple[3];
    (void)self;

    r_dmp = mpu_dmp_get_data(&pitch,&roll,&yaw);

    if(r_dmp == 0)    //得到欧拉角数据
    {
        tuple[0] = mp_obj_new_float(pitch);
        tuple[1] = mp_obj_new_float(roll);
        tuple[2] = mp_obj_new_float(yaw);
        pre_pitch = pitch;
        pre_roll = roll;
        pre_yaw = yaw;
    }
    else
    {
        tuple[0] = mp_obj_new_float(pre_pitch);
        tuple[1] = mp_obj_new_float(pre_roll);
        tuple[2] = mp_obj_new_float(pre_yaw);
    }

    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getDMPdata_obj, machine_mpu_getDMPdata);

/// \method getAccel_x()
/// mpu6050 get Accelerometer x.
STATIC mp_obj_t machine_mpu_getAccel_x(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short aacx,aacy,aacz;       //加速度传感器原始数据
    (void)self;

    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据
    // aRes = getAres(Ascale);
    ax = (float)acosf((aacx-accelOffset[0])/16384.0)*57.29577;

    if (ax < 0.05 && ax > -0.05)
    {
        ax = 0.05;
    }

    return mp_obj_new_float(ax);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getAccel_x_obj, machine_mpu_getAccel_x);

/// \method getAccel_y()
/// mpu6050 get Accelerometer y.
STATIC mp_obj_t machine_mpu_getAccel_y(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short aacx,aacy,aacz;       //加速度传感器原始数据
    (void)self;

    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据
    // aRes = getAres(Ascale);
    ay = (float)acosf((aacy-accelOffset[1])/16384.0)*57.29577;

    if (ay < 0.05 && ay > -0.05)
    {
        ay = 0.05;
    }

    return mp_obj_new_float(ay);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getAccel_y_obj, machine_mpu_getAccel_y);

/// \method getAccel_z()
/// mpu6050 get Accelerometer z.
STATIC mp_obj_t machine_mpu_getAccel_z(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short aacx,aacy,aacz;       //加速度传感器原始数据
    (void)self;

    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //得到加速度传感器数据

    // aRes = getAres(Ascale);
    az = (float)acosf((aacz-accelOffset[2])/16384.0)*57.29577;

    if (az < 0.05 && az > -0.05)
    {
        az = 0.05;
    }

    return mp_obj_new_float(az);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getAccel_z_obj, machine_mpu_getAccel_z);

/// \method getGyro_x()
/// mpu6050 get Gyroscope x.
STATIC mp_obj_t machine_mpu_getGyro_x(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short gyrox,gyroy,gyroz;    //陀螺仪原始数据
    (void)self;

    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据

    // gRes = getGres(Gscale);
    gx = (float)((gyrox-gyroOffset[0])/163.84);

    return mp_obj_new_float(gx);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getGyro_x_obj, machine_mpu_getGyro_x);

/// \method getGyro_y()
/// mpu6050 get Gyroscope y.
STATIC mp_obj_t machine_mpu_getGyro_y(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short gyrox,gyroy,gyroz;    //陀螺仪原始数据
    (void)self;

    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据

    // gRes = getGres(Gscale);
    gy = (float)((gyroy-gyroOffset[1])/163.84);

    return mp_obj_new_float(gy);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getGyro_y_obj, machine_mpu_getGyro_y);

/// \method getGyro_z()
/// mpu6050 get Gyroscope z.
STATIC mp_obj_t machine_mpu_getGyro_z(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;
    short gyrox,gyroy,gyroz;    //陀螺仪原始数据
    (void)self;

    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //得到陀螺仪数据

    // gRes = getGres(Gscale);
    gz = (float)((gyroz-gyroOffset[2])/163.84);

    return mp_obj_new_float(gz);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_getGyro_z_obj, machine_mpu_getGyro_z);

/// \method lowPowerOnlyAccel()
/// mpu6050 low power only Accel registers MPU6050.
STATIC mp_obj_t machine_mpu_lowPowerOnlyAccel(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;

    (void)self;

    LowPowerAccelOnlyMPU6050();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_lowPowerOnlyAccel_obj, machine_mpu_lowPowerOnlyAccel);

/// \method selfTest()
/// mpu6050 self test MPU6050.
STATIC mp_obj_t machine_mpu_selfTest(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;

    mp_obj_t tuple[6];
    (void)self;

    MPU6050SelfTest(SelfTest);    //得到self数据

    tuple[0] = mp_obj_new_float(SelfTest[0]);
    tuple[1] = mp_obj_new_float(SelfTest[1]);
    tuple[2] = mp_obj_new_float(SelfTest[2]);
    tuple[3] = mp_obj_new_float(SelfTest[3]);
    tuple[4] = mp_obj_new_float(SelfTest[4]);
    tuple[5] = mp_obj_new_float(SelfTest[5]);

    return mp_obj_new_tuple(6, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_selfTest_obj, machine_mpu_selfTest);

/// \method calibrateMPU()
/// mpu6050 calibrate MPU6050.
STATIC mp_obj_t machine_mpu_calibrateMPU(mp_obj_t self_in) {
    machine_mpu_obj_t *self = self_in;

    mp_obj_t tuple[6];
    (void)self;

    calibrateMPU6050(gyroBias,accelBias);    //得到bias数据

    tuple[0] = mp_obj_new_float(accelBias[0]);
    tuple[1] = mp_obj_new_float(accelBias[1]);
    tuple[2] = mp_obj_new_float(accelBias[2]);
    tuple[3] = mp_obj_new_float(gyroBias[0]);
    tuple[4] = mp_obj_new_float(gyroBias[1]);
    tuple[5] = mp_obj_new_float(gyroBias[2]);

    return mp_obj_new_tuple(6, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_mpu_calibrateMPU_obj, machine_mpu_calibrateMPU);


STATIC const mp_rom_map_elem_t machine_mpu_locals_dict_table[] = {
    // instance methods
    // class methods
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&mp_machine_mpu_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_machine_mpu_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_initMPU), MP_ROM_PTR(&mp_machine_mpu_initMPU_obj) },
    { MP_ROM_QSTR(MP_QSTR_DMPinit), MP_ROM_PTR(&mp_machine_mpu_DMPinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_getTemp), MP_ROM_PTR(&mp_machine_mpu_getTemp_obj) },
    { MP_ROM_QSTR(MP_QSTR_getAccel), MP_ROM_PTR(&mp_machine_mpu_getAccel_obj) },
    { MP_ROM_QSTR(MP_QSTR_getGyros), MP_ROM_PTR(&mp_machine_mpu_getGyros_obj) },
    { MP_ROM_QSTR(MP_QSTR_getDMPdata), MP_ROM_PTR(&mp_machine_mpu_getDMPdata_obj) },
    { MP_ROM_QSTR(MP_QSTR_getAccel_x), MP_ROM_PTR(&mp_machine_mpu_getAccel_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_getAccel_y), MP_ROM_PTR(&mp_machine_mpu_getAccel_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_getAccel_z), MP_ROM_PTR(&mp_machine_mpu_getAccel_z_obj) },
    { MP_ROM_QSTR(MP_QSTR_getGyro_x), MP_ROM_PTR(&mp_machine_mpu_getGyro_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_getGyro_y), MP_ROM_PTR(&mp_machine_mpu_getGyro_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_getGyro_z), MP_ROM_PTR(&mp_machine_mpu_getGyro_z_obj) },
    { MP_ROM_QSTR(MP_QSTR_lowPowerOnlyAccel), MP_ROM_PTR(&mp_machine_mpu_lowPowerOnlyAccel_obj) },
    { MP_ROM_QSTR(MP_QSTR_selfTest), MP_ROM_PTR(&mp_machine_mpu_selfTest_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibrateMPU), MP_ROM_PTR(&mp_machine_mpu_calibrateMPU_obj) },
    { MP_ROM_QSTR(MP_QSTR_setAccelOffset), MP_ROM_PTR(&mp_machine_mpu_setAccelOffset_obj) },
    { MP_ROM_QSTR(MP_QSTR_setGyrosOffset), MP_ROM_PTR(&mp_machine_mpu_setGyrosOffset_obj) },

};

STATIC MP_DEFINE_CONST_DICT(machine_mpu_locals_dict, machine_mpu_locals_dict_table);

const mp_obj_type_t machine_mpu_type = {
    { &mp_type_type },
    .name = MP_QSTR_MPU6050,
    .make_new = machine_mpu_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_mpu_locals_dict,
    .print = machine_mpu_print,
};

/*lint --flb "Leave library region" */
#endif // MICROPY_PY_MACHINE_MPU6050