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

from Rs485 import *
import time

#ttyUSB2 - 9600
port = Rs485(2,9600)
if port.openPort() == False :
    print("Error opening serial port")
    exit()


#hago un envio para que se acomode la rts
ba = bytearray()
ba.append(0)
port.sendPacket(ba,1)
#_______________________________________



while True:
    data = port.receivePacketWithTimeout(100)
    if len(data)>0:
        out = data
        while True:
            data = port.receivePacketWithTimeout(10)
            if len(data)>0:
                for b in data:
                    out.append(b)
            else:
                print("------------------------ "+time.strftime("%H:%M:%S")+" -------------------------")
                print ''.join('{:02x}'.format(x) for x in out)
                print(out)

                port.sendPacket(out,len(out))
                break

