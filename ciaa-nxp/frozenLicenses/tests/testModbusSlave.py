import pyb
import ModBus

class UartMock:
    def write(self,data):
        print("envio:")
        for b in data:
            print(b)

    def readall(self):
        print("recibi:")
        b = bytearray()
        
        b.append(ord(':'))
        b.append(ord('5'))
        b.append(ord('5'))
        '''
        b.append(ord('0'))
        b.append(ord('3'))
        b.append(ord('4'))
        b.append(ord('0'))
        b.append(ord('0'))
        b.append(ord('0'))
        b.append(ord('0'))
        b.append(ord('0'))        
        b.append(ord('0'))
        b.append(ord('2'))
        b.append(ord('6'))
        b.append(ord('6'))
        '''

        b.append(ord('0'))
        b.append(ord('6'))
        b.append(ord('4'))
        b.append(ord('0'))
        b.append(ord('0'))
        b.append(ord('0'))

        b.append(ord('1'))
        b.append(ord('1'))
        b.append(ord('2'))
        b.append(ord('2'))
                
        b.append(ord('3'))
        b.append(ord('2'))

        b.append(ord('\r'))
        b.append(ord('\n'))
             
        print(b)
        return b
        
    def get_baudrate(self):
        return 9600
    def any(self):
        return True
        
uart = UartMock()


mappedRegs = {0x4000 : 0x1112 , 0x4001 : 0x1314}

modbus = ModBus.Slave(uart,0x55,mappedRegs,mode=ModBus.MODE_ASCII)
print(modbus)

print(mappedRegs)

#while True:
modbus.receive()
    

print(mappedRegs)


