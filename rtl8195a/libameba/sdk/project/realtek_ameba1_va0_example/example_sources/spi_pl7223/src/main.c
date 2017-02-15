/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include <stdio.h>
#include <math.h>
#include "device.h"
#include "main.h"
#include "spi_api.h"
#include "gpio_api.h"

// SPI0
#define SPI0_MOSI  	PC_2
#define SPI0_MISO  	PC_3
#define SPI0_SCLK  	PC_1
#define SPI0_CS    	PC_0

#define GPIO_RESET	PB_4
#define GPIO_CS		PB_5

//--------------------------------------------------------------------------------------------

#define	READ_PL7223			0x4000
#define	WRITE_PL7223		0x8000
#define	DSPSTATUS_PL7223	0xF000
#define DUM_PL7223			0x00	//Dummy Data

unsigned char SPDAT;							// simulate example code
unsigned char DSP_STATUS=0;
unsigned char Read_Data_PL7223[146];			// Read_Data; 256Bytes=1Page
unsigned char Write_Data_PL7223[146];			// Write_Data; 256Bytes=1Page
unsigned char Cmd_RD=0;


long EE_Temp = 0;
float VA_rms=0;
float IA_rms=0;
float PA=0;
float SA=0;
float QA=0;
float PF_A=0;
float Theta_A=0;
float Frequency=0;
int Sample_cnt0=0;
int ZCC_cnt=0;
int ZCC_Start=0;
int ZCC_Stop=0;

void Initial_SPI_PL7223(void);
void SPI_PL7223_SEND(unsigned char);
void SPI__PL7223_Read_Status(void);
void SPI_PL7223_DELY(int);
void SPI_PL7223_Reset(void);
void SPI_PL7223_Read(unsigned char*, unsigned int, unsigned int);
void SPI_PL7223_Write(unsigned char*, unsigned int, unsigned int);
void SPI_PL7223_Masurement(void);
void SPI_PL7223_RelayControl(int);

static spi_t spi0_master;
static gpio_t gpio_reset;
static gpio_t gpio_cs;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{

	gpio_init(&gpio_reset, GPIO_RESET);
	gpio_mode(&gpio_reset, PullUp);
	gpio_dir(&gpio_reset, PIN_OUTPUT);

	gpio_init(&gpio_cs, GPIO_CS);
	gpio_mode(&gpio_cs, PullUp);
	gpio_dir(&gpio_cs, PIN_OUTPUT);
	
    spi_init(&spi0_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi0_master, 8, 3, 0);
	spi_frequency(&spi0_master, 800000);
	
	do
	{
		SPI_PL7223_Reset();
		SPI_PL7223_Read(&Read_Data_PL7223[0],0x3860,1);//DSP version :20130322 ver02, 0x3860=0x04
                                                                 //DSP version :20141009 ver01, 0x3860=0x03
	}while( ((Read_Data_PL7223[0]) != 0x04) && ((Read_Data_PL7223[0]) != 0x03) );
	
	SPI_PL7223_DELY(120000);
	SPI_PL7223_RelayControl(0);		// OFF
	SPI_PL7223_DELY(120000);
	
	do{
	// As below is read DSP buffer process every time (144 byte)
	SPI__PL7223_Read_Status();
	SPI_PL7223_Read(&Read_Data_PL7223[0],0x3000,144); // 0x3000~0x308F     //144 byte
	SPI_PL7223_Read(&Read_Data_PL7223[144],0x3809,2); // Sample_cnt0
	SPI_PL7223_Masurement();
	
	SPI_PL7223_DELY(600000);
	SPI_PL7223_RelayControl(1);		// ON
	SPI_PL7223_DELY(120000);
	
	SPI__PL7223_Read_Status();
	SPI_PL7223_Read(&Read_Data_PL7223[0],0x3000,144); // 0x3000~0x308F     //144 byte
	SPI_PL7223_Read(&Read_Data_PL7223[144],0x3809,2); // Sample_cnt0
	SPI_PL7223_Masurement();
	
	SPI_PL7223_DELY(600000);
	SPI_PL7223_RelayControl(0);		// OFF
	SPI_PL7223_DELY(120000);
	}while(1);
}

//--------------------------------------------------------------------------------------------//
void SPI_PL7223_RelayControl(int sw)
{
	#define RELAY_MASK (1<<5)
	SPI_PL7223_Read(&Read_Data_PL7223[0],0x380F,1);
	if(!sw)
		Read_Data_PL7223[0] &= (~RELAY_MASK);
	else
		Read_Data_PL7223[0] |= RELAY_MASK;
	SPI_PL7223_Write(&Read_Data_PL7223[0],0x380F,1);	
}

//--------------------------------------------------------------------------------------------//
void SPI_PL7223_Reset(void)
{
	gpio_write(&gpio_cs, 0);
	SPI_PL7223_DELY(500);         //need delay 10ms
	gpio_write(&gpio_reset, 1);
	SPI_PL7223_DELY(500);        //need delay 10ms
	gpio_write(&gpio_reset, 0);
	SPI_PL7223_DELY(500);	    //need delay 10ms
	gpio_write(&gpio_reset, 1);
	SPI_PL7223_DELY(500);       //need delay 10ms
	gpio_write(&gpio_cs, 1);
	SPI_PL7223_DELY(300);
}

//--------------------------------------------------------------------------------------------//
void SPI__PL7223_Read_Status(void)
{
	gpio_write(&gpio_cs, 0);
	SPI_PL7223_SEND((unsigned char)(DSPSTATUS_PL7223 >> 8)& 0xFF);				// RDSR command 
	SPI_PL7223_SEND((unsigned char)(DSPSTATUS_PL7223& 0x00FF));				// RDSR command 

	//check DSP flag state (byte)
	do
	{
		SPI_PL7223_SEND(DUM_PL7223); 
		DSP_STATUS=SPDAT;
	}while((DSP_STATUS & 0x80) == 0x00);  		 // Bit7=1 is Ready
	gpio_write(&gpio_cs, 1);
}

//--------------------------------------------------------------------------------------------//
void SPI_PL7223_Write(unsigned char* buf, unsigned int addr, unsigned int len)
{
	unsigned int i;
	gpio_write(&gpio_cs, 0);
	addr |= WRITE_PL7223;		// Write command 
	SPI_PL7223_SEND((unsigned char)(addr >> 8)& 0xFF);	// Write middle byte address
	SPI_PL7223_SEND((unsigned char)(addr & 0xFF));// Write low byte address
	for (i = 0; i < len ; i++){
		SPI_PL7223_SEND(buf[i]);
	}
	gpio_write(&gpio_cs, 1);
	SPI_PL7223_DELY(3);			// for CS:Hi to Low need 100nsec, Delay-Time 27usec

}
//--------------------------------------------------------------------------------------------//

void SPI_PL7223_Read(unsigned char* buf, unsigned int addr, unsigned int len)
{
	static unsigned int i;

	gpio_write(&gpio_cs, 0);

	addr |= READ_PL7223;		// Read command
	SPI_PL7223_SEND((unsigned char)(addr >> 8)& 0xFF);	    // Write middle byte address
	SPI_PL7223_SEND((unsigned char)(addr & 0x00FF));       // Write low byte address

	for(i=0;i<len;i++){			// Read 256 Bytes/Page to Flash Memory
		SPI_PL7223_SEND(DUM_PL7223);						// Send Dummy Data to Read righ Data
		buf[i] = SPDAT;	// Read SPIDAT and clear TX complete flag
	}

	gpio_write(&gpio_cs, 1);

}
//--------------------------------------------------------------------------------------------//
void SPI_PL7223_Read_Status(void)
{

	gpio_write(&gpio_cs, 0);
	SPI_PL7223_SEND((unsigned char)(DSPSTATUS_PL7223 >> 8)& 0xFF);				// RDSR command 
	SPI_PL7223_SEND((unsigned char)(DSPSTATUS_PL7223& 0x00FF));				// RDSR command 

	do
	{
		SPI_PL7223_SEND(DUM_PL7223);
		DSP_STATUS=SPDAT;
	}while((DSP_STATUS & 0x80) == 0x00);  		 // Bit7=1 is Ready

	gpio_write(&gpio_cs, 1);

}

//--------------------------------------------------------------------------------------------//

void SPI_PL7223_Masurement(void)
{

	//Vrms address : 0x3002~0x3003
	// VA_rms = (Read_Data_PL7223[3]*256+Read_Data_PL7223[2])/64;
	EE_Temp =  Read_Data_PL7223[3];
	EE_Temp =  EE_Temp << 8;
	EE_Temp += Read_Data_PL7223[2];
	VA_rms = (float)EE_Temp/64.00;          


	//Irms address : 0x3008~0x3009
	// IA_rms = Read_Data_PL7223[3]+Read_Data_PL7223[2]/256;
	EE_Temp =  Read_Data_PL7223[8];
	IA_rms = (float)EE_Temp/256.00;
	EE_Temp = Read_Data_PL7223[9];
	IA_rms = IA_rms + (float)EE_Temp;


	//Active address : 0x3078~0x307D
	// PA = Read_Data_PL7223[124]*256+Read_Data_PL7223[123];
	EE_Temp =  Read_Data_PL7223[124];
	EE_Temp =  EE_Temp << 8;
	EE_Temp += Read_Data_PL7223[123];		
	PA = (float)EE_Temp;

	//PF Calculate
	// SA = VA_rms*IA_rms;
	SA = VA_rms*IA_rms;
	// PF_A = PA/SA
	PF_A = SA==0? 0: PA/SA;
	Theta_A = acos(PF_A);
	QA = SA * sin(Theta_A);
	if(IA_rms==0)
		Theta_A = 0;
	else
		Theta_A = Theta_A * (180.00/(3.141592653589));
	    
	/** Frequency = [Sample_cnt0/(ZCC_STOP-ZCC_START)]*[(ZCC_CNT-1)/2] */
	Sample_cnt0 = Read_Data_PL7223[145];						// Sample_cnt01
	Sample_cnt0 = Sample_cnt0 <<8;
	Sample_cnt0 += Read_Data_PL7223[144];						// Sample_cnt00

	ZCC_cnt = Read_Data_PL7223[91];								// ZCC_cnt1
	ZCC_cnt = ZCC_cnt <<8;						
	ZCC_cnt += Read_Data_PL7223[90];							// ZCC_cnt0

	ZCC_Stop = Read_Data_PL7223[97];							// ZCC_STOP1
	ZCC_Stop = ZCC_Stop <<8;			
	ZCC_Stop += Read_Data_PL7223[96];							// ZCC_STOP0

	ZCC_Start = Read_Data_PL7223[103];							// ZCC_START1
	ZCC_Start = ZCC_Start <<8;								
	ZCC_Start += Read_Data_PL7223[102];							// ZCC_START0
	
	Frequency = (float)((float)Sample_cnt0 / (ZCC_Stop - ZCC_Start)) * (((float)ZCC_cnt - 1.0) / 2);

#define UART_Display(name) printf(#name" %d.%d\n\r", (int)(name*1000)/1000, (int)(name*1000)%1000)
	UART_Display(VA_rms);
	UART_Display(IA_rms);
	UART_Display(Frequency);
	UART_Display(PA);
	UART_Display(QA);
	UART_Display(SA);
	UART_Display(PF_A);
	UART_Display(Theta_A);	
}
//--------------------------------------------------------------------------------------------//
void SPI_PL7223_DELY(int dely_cnt)	// MCUCLK 4MHz, Delay-Time 9usec/clock
{  
	HalDelayUs(dely_cnt*20);
}

//--------------------------------------------------------------------------------------------//
void SPI_PL7223_SEND(unsigned char spicmd)
{
	SPDAT = (char)spi_master_write(&spi0_master, (int)spicmd);
}
//--------------------------------------------------------------------------------------------//
		

