/*
    <Rs485 Dinamic Library>
    Copyright (C) <2013>  <Ernesto Gigliotti>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"
#include "Rs485Layer.h"


int rs485OpenPort(Port * pPort)
{
        return OpenComport(pPort->cport_nr, pPort->bdrate);
}

int rs485SendPacket(Port * pPort,unsigned char * buff,int size)
{
	int r;

#ifdef __linux__
        //setDTR(pPort->cport_nr, 1);
	setRTS(pPort->cport_nr, 1);
#endif

	r =  SendBuf(pPort->cport_nr, buff,size);

	if(r!=-1)
	{
		// Recibo lo mismo que transmiti, espero 100ms maximo
		unsigned char bufferRx[4096];
		int timeout=100;
		while( (rs485Receive(pPort,bufferRx,sizeof(bufferRx))<size) && (timeout>0) )
		{
			// Delay 1ms
			#ifdef _WIN32
				Sleep(1);
			#else
				usleep(1000);
			#endif
			//__________
			timeout--;
		}
		//______________________________
		if(timeout==0)
			r=-1;
	}

#ifdef __linux__
	setRTS(pPort->cport_nr, 0);
#endif

	return r;
}

int rs485Receive(Port * pPort,unsigned char * buff,int size)
{
	return PollComport(pPort->cport_nr, buff, size);
}

int rs485ReceiveWithTimeout(Port * pPort,unsigned char * bufferRx,int size,int timeout)
{
	  int n ;
	  while( ( (n = rs485Receive(pPort,bufferRx,size))<=0) && (timeout>0) )
	  {
		  // Delay 1ms
			#ifdef _WIN32
				Sleep(1);
			#else
				usleep(1000);
			#endif
		  //__________
		  timeout--;
	  }
	  //__________________
	  if(timeout!=0)
          {
                 return n;
          }
	  return 0;
}

void rs485ClosePort(Port * pPort)
{
	CloseComport(pPort->cport_nr);
	
}
