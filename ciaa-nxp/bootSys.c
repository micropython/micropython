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
void boot_writeScript(char* script,int scriptLen);


void boot(void)
{
	mp_hal_milli_delay(1000); // wait for power stabilization

	Board_UARTPutChar(NACK); // send NACK

	//format: SOH 01 FE Data[128] CSUM

	unsigned char bufferRx[160];
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
			Board_UARTPutChar(ACK); // send ACK

			// write in file
			boot_writeScript(bufferRx+2,128);

		}
		else if(inbyte==EOT)
		{
			Board_UARTPutChar(ACK); // send ACK
			break;
		}
	}

}

int boot_getChar(void)
{
	int i;
	for(i=0; i<400000;i++){
		int inbyte  = Board_UARTGetChar();
		if(inbyte!=EOF)
			return inbyte;
	}
	return EOF;
}


void boot_writeScript(char* script,int scriptLen)
{
        FIL fp;
        f_open(&fp, "/flash/Main.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, script, scriptLen - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);
}
