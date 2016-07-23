#    <Rs485 Dinamic Library Demo>
#    Copyright (C) <2013>  <Ernesto Gigliotti>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

from ctypes import *
libRs485 = CDLL("./librs485_linux_i386.so")

class Port(Structure):
        _fields_ = [('cport_nr',c_uint),('bdrate',c_uint)]

class Rs485:

   	def __init__(self,portN,baud) :
		self.port = Port(portN,baud)
		self.pPort = POINTER(Port)(self.port)
		return
	
	def openPort(self) :
		ret = libRs485.rs485OpenPort(self.pPort)
		if ret != 0 :
	    		return False
		return True

	# recibe un bytearray y un int con el size
	# devuelve la cantidad de bytes transmitidos
    	def sendPacket(self,ba,len):
		# convertimos el bytearray en un array de ctypes
   		arrayType = c_ubyte*len
    		array = arrayType.from_buffer(ba)
		# transmite el packet
		return libRs485.rs485SendPacket(self.pPort,array,len)
    	
	# recibe el timeout de espera
	# devuelve un bytearray con el packet recibido
    	def receivePacketWithTimeout(self,timeout):
		ArrayType = c_ubyte*4096 # creo el tipo del buffer
		buffRx = ArrayType() # creo el buffer
		n = libRs485.rs485ReceiveWithTimeout(self.pPort,buffRx,4096,timeout)
   		ba = bytearray()
		if n > 0 :
    			i=0
			while i<n:
        			ba.append(buffRx[i])
				i=i+1
		return ba

    	def receivePacket(self):
                ArrayType = c_ubyte*4096 # creo el tipo del buffer
                buffRx = ArrayType() # creo el buffer
 		n = libRs485.rs485Receive(self.pPort,buffRx,4096)
                ba = bytearray()
                if n > 0 :
                        i=0
                        while i<n:
                                ba.append(buffRx[i])
                                i=i+1
                return ba


    	def closePort(self):
		libRs485.rs485ClosePort(self.pPort)
		return

