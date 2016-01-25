#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lib/fatfs/ff.h"
#include "board.h"
#include "ciaanxp_mphal.h"

#define ACK	0x06
#define NACK	0x15
#define EOT	0x04
#define SOH	0x01

int boot_getChar(void);
void boot_writeScript(const unsigned char* script,int scriptLen,int mode);
int searchXmodemEndOfData(unsigned char* data,int maxIndex);
int receiveXmodemPacket(unsigned char* bufferRx);
int checkCrc(unsigned char* buffer, int len);

void boot(void)
{
	mp_hal_milli_delay(1000); // wait for power stabilization

	Board_UARTPutChar(NACK); // send NACK

	//format: SOH 01 FE Data[128] CSUM

	unsigned char bufferRx[160];
	int size;
	int mode=0;
	while(1)
	{
		size = receiveXmodemPacket(bufferRx);
		if(size==0)
			break;
		else
		{
			// receive packet
			// write in file
			if(size>128)
			{
				// check CRC
				int crcOk = checkCrc(bufferRx+3,128+1);

				/* // test echo
				int j=0;
				for(j=0; j<size; j++)
					Board_UARTPutChar(bufferRx[j]);
				*/

				if(crcOk==1)
				{
                    int size = searchXmodemEndOfData(bufferRx+3,128);
                    boot_writeScript(bufferRx+3,size,mode);
					mode=1;
					Board_UARTPutChar(ACK); // send ACK
				}
				else
					Board_UARTPutChar(NACK); // send NACK
			}
			else
				Board_UARTPutChar(ACK); // send ACK

		}
	}

	 // Test debug
	/*
	FIL fp;
	UINT n;
	char aux[160];
	f_open(&fp, "/flash/Main.py", FA_READ);
        char dataread[600];
        f_read (&fp, dataread, 600, &n);
        sprintf(aux,"lei:%d bytes\r\n",n);
        Board_UARTPutSTR(aux);

        sprintf(aux,"primeros caracteres: 0x%x - 0x%x - 0x%x \r\n",dataread[0],dataread[1],dataread[2]);
        Board_UARTPutSTR(aux);

        sprintf(aux,"ultimos 32 caracteres: \r\n");
        Board_UARTPutSTR(aux);
	int k=32;
	while(k>0)
	{
		sprintf(aux,"0x%x -",dataread[n-k]);
	        Board_UARTPutSTR(aux);
		k--;
	}
        sprintf(aux,"\r\n");
        Board_UARTPutSTR(aux);


        Board_UARTPutSTR(dataread);
        Board_UARTPutSTR("FIN escribo archivo");
	f_close(&fp);
	*/

	return;
}

int boot_getChar(void)
{
	int i;
	for(i=0; i<800000;i++){
		int inbyte  = Board_UARTGetChar();
		if(inbyte!=EOF)
			return inbyte;
	}
	return EOF;
}


int searchXmodemEndOfData(unsigned char* data,int maxIndex)
{
	int i=0;
	while(i<maxIndex)
	{
		if(data[i]==0x1A)
			return i;
		i++;
	}
	return maxIndex;
}

int receiveXmodemPacket(unsigned char* bufferRx)
{
        int i=0;
        while(1)
        {
                int inbyte = boot_getChar();
                if(inbyte==EOF)
                        break;
                bufferRx[i] = (unsigned char)inbyte;
                i++;
        }
        return i;
}

int checkCrc(unsigned char* buffer, int len)
{
	int crc=0;
	int i;
	for(i=0; i<(len-1);i++)
		crc+=buffer[i];

	if((crc&0x00FF)==buffer[len-1])
		return 1;
	return 0;
}

void boot_writeScript(const unsigned char* script,int scriptLen,int mode)
{
	FIL fp;
	if(mode==0) {
        f_open(&fp, "/flash/Main.py", FA_WRITE | FA_CREATE_ALWAYS);
	} else {
		f_open(&fp, "/flash/Main.py", FA_WRITE | FA_OPEN_ALWAYS);
		f_lseek(&fp, f_size(&fp));
	}

	UINT n;

	f_write(&fp, script, scriptLen , &n);
	f_close(&fp);
	// TODO: Check n
}
