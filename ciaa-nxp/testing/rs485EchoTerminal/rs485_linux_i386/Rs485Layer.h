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

#ifndef RS485LAYER_H_
#define RS485LAYER_H_

typedef struct
{
	int cport_nr;
	int bdrate;
}Port;

int rs485OpenPort(Port * pPort);
int rs485SendPacket(Port * pPort,unsigned char * buff,int size);
int rs485Receive(Port * pPort,unsigned char * buff,int size);
int rs485ReceiveWithTimeout(Port * pPort,unsigned char * bufferRx,int size,int timeout);
void rs485ClosePort(Port * pPort);

#endif /* RS485LAYER_H_ */
