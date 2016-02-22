
#include "device.h"
#include "PinNames.h"

#include "basic_types.h"
#include "diag.h" 
#include "osdep_api.h"

#include "i2c_api.h"
#include "pinmap.h"
//#include "rtl_lib.h"
#include "main.h"

#define MBED_I2C_MTR_SDA    PB_3
#define MBED_I2C_MTR_SCL    PB_2
#define MBED_I2C_INTB		PA_5
#define MBED_I2C_SLAVE_ADDR0    0x49
#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_DATA_MAX_LENGTH     20

uint8_t i2cdata_write[I2C_DATA_MAX_LENGTH];
uint8_t i2cdata_read[I2C_DATA_MAX_LENGTH];
uint16_t cmd;

i2c_t   i2cmaster;
//sensor command     
#define WAKE_UP 0x1102
#define CHIP_REFRESH1 0xFD8E
#define CHIP_REFRESH2 0xFE22
#define CHIP_REFRESH3 0xFE02
#define CHIP_REFRESH4 0xFD00
#define PS_MODE 0x0002
#define ALS1_MODE 0x0072
#define ALS2_MODE 0x503E
#define ALS3_MODE 0x583E
#define POWER_UP 0x1102 
#define CHIP_RESET 0x1100
#define CHANGE_TIME 0x0851
#define SETTING_1 0x0F19
#define SETTING_2 0x0D10
#define INT 0x3022

char    i2cdatasrc[5] = {0x1B, 0x1E, 0x1F, 0x80, 0x88};


static void ePL_WriteCommand(uint16_t cmd)
{
	i2cdata_write[0] = (uint8_t)(cmd >>8);
	i2cdata_write[1] = (uint8_t)(cmd&0xFF);
  	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdata_write[0], 2, 1);   
}  


void main(void)
{
        int result;
        int i;
        int flag = 0;
        char intertupt;

	DiagPrintf("Sensor_Init \r\n");
	i2c_init(&i2cmaster, MBED_I2C_MTR_SDA ,MBED_I2C_MTR_SCL);
	i2c_frequency(&i2cmaster,MBED_I2C_BUS_CLK);
 
        ePL_WriteCommand(WAKE_UP);
        ePL_WriteCommand(CHIP_REFRESH1);
        ePL_WriteCommand(CHIP_REFRESH2);
        ePL_WriteCommand(CHIP_REFRESH3);
        ePL_WriteCommand(CHIP_REFRESH4);
        
	ePL_WriteCommand(PS_MODE);
        
        ePL_WriteCommand(SETTING_1);
        ePL_WriteCommand(SETTING_2);
        
        
	ePL_WriteCommand(CHIP_RESET);
        
        ePL_WriteCommand(POWER_UP);
        Mdelay(240);
        while(1){
            //ePL_WriteCommand(DATA_LOCK);
            i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[0], 1, 1);  
            i2c_read(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char*)&i2cdata_read[0], 2, 1);
            i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[1], 1, 1);  
            i2c_read(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char*)&i2cdata_read[1], 2, 1);
            i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[2], 1, 1);  
            i2c_read(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char*)&i2cdata_read[2], 2, 1);
            //printf("PS LOW: %d\n", i2cdata_read[1]);
            //printf("PS HIGH: %d\n", i2cdata_read[2]);
            flag = (i2cdata_read[0] & 8)? 1:0;
            int ret = (i2cdata_read[0] & 4)? 1:0;
            //printf("flag: %d\n", flag);
            //printf("ret: %d\n", ret);
            
          if(flag){
            printf("the object is far\n");
          }
          else
          {
            printf("the object is near\n");
          }
        
        //ePL_WriteCommand(POWER_UP);
        Mdelay(1000);
        
        }

        
	

       
       
  
}
