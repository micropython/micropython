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
void boot_writeScript(char* script,int scriptLen,int mode);

int searchXmodemEndOfData(char* data,int maxIndex);

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
        //Board_UARTPutChar(ACK); // send ACK
	return i;
}

void boot(void)
{
	mp_hal_milli_delay(1000); // wait for power stabilization

	/*
	const char* data = "print('hola mundo desde boot');\r\n";
	boot_writeScript(data,strlen(data));

	return;
	*/

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
                        	int size = searchXmodemEndOfData(bufferRx+3,128);
                        	boot_writeScript(bufferRx+3,size,mode);
				mode=1;
			}
			Board_UARTPutChar(ACK); // send ACK
		}
	}
	return;


	/*

	int inbyte;
	while(1) {
		inbyte = boot_getChar();
		if(inbyte==EOF)
			return; // no script from IDE

		if(inbyte==SOH) // start packet
		{
			int i;
			for(i=0; i<(128+3);i++)
			{
				inbyte = boot_getChar();
		                if(inbyte==EOF) {
                		        return; // no script from IDE
				}
				bufferRx[i]=(unsigned char)inbyte;
			}
			// TODO check CRC

			// send ACK
			p_hal_milli_delay(10);
			Board_UARTPutChar(ACK); // send ACK

			// write in file
			int size = searchXmodemEndOfData(bufferRx+2,128);
			boot_writeScript(bufferRx+2,size);

		}
		else if(inbyte==EOT)
		{
			Board_UARTPutChar(ACK); // send ACK
			break;
		}
	}
	*/
}

int boot_getChar(void)
{
	int i;
	//for(i=0; i<400000;i++){
	for(i=0; i<800000;i++){
		int inbyte  = Board_UARTGetChar();
		if(inbyte!=EOF)
			return inbyte;
	}
	return EOF;
}


int searchXmodemEndOfData(char* data,int maxIndex)
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
void boot_writeScript(char* script,int scriptLen,int mode)
{
	//Board_UARTPutSTR("escribo archivo... ");
	/*
	FRESULT res = f_mkfs("/flash", 0, 0);
        if (res == FR_OK) {
            // success creating fresh LFS
        } else {
            __BKPT(0);
        }

        // set label
        f_setlabel("/flash/pybflash");
	*/

        FIL fp;
	//Board_UARTPutSTR("1");
	if(mode==0)
        	f_open(&fp, "/flash/Main.py", FA_WRITE | FA_CREATE_ALWAYS);
	else
	{
		f_open(&fp, "/flash/Main.py", FA_WRITE | FA_OPEN_ALWAYS);
		f_lseek(&fp, f_size(&fp)+1);
	}
        UINT n;
	//Board_UARTPutSTR("2");
        f_write(&fp, script, scriptLen  /* don't count null terminator */, &n);
	//f_sync(&fp);


	//Board_UARTPutSTR("escribi archivo");
	//char aux[160];
	//sprintf(aux,"escribo:%s - n:%d\r\n",script,n);
	//Board_UARTPutSTR(aux);

        //Board_UARTPutSTR("3");
        f_close(&fp);
        //Board_UARTPutSTR("4");

	/*
        // TODO check we could write n bytes
	f_open(&fp, "/flash/Main.py", FA_READ);
	char dataread[256];
	f_read (&fp, dataread, 256, &n);
	sprintf(aux,"lei:%d bytes\r\n",n);
	Board_UARTPutSTR(aux);
	sprintf(aux,"primeros caracteres: 0x%x - 0x%x - 0x%x \r\n",dataread[0],dataread[1],dataread[2]);
	Board_UARTPutSTR(aux);

	Board_UARTPutSTR(dataread);
	Board_UARTPutSTR("FIN escribo archivo");
	*/
}
