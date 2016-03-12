import pyb
import ModBus

class UartMock:
    def write(self,data):
        print("envio:")
        for b in data:
            print(b)

    def read(self,numberOfBytes):
        print("recibo:")
        b = bytearray()

        b.append(ord(':'))
        b.append(ord('5'))
        b.append(ord('5'))
        b.append(ord('0'))
        b.append(ord('3'))
        b.append(ord('0'))
        b.append(ord('2'))
        b.append(ord('1'))
        b.append(ord('1'))
        b.append(ord('2'))
        b.append(ord('2'))
        b.append(ord('7'))
        b.append(ord('3'))
        b.append(ord('\r'))
        b.append(ord('\n'))
             
        print(b)
        return b
        
    def get_baudrate(self):
        return 9600
    def any(self):
        return True
        
uart = UartMock()


modbus = ModBus.Instrument(uart,0x55,mode=ModBus.MODE_ASCII)
print(modbus)
'''
print("hago read bit")
val = modbus.read_bit(1,2) #
print("respuesta :")
print(val)
'''
#modbus.write_bit(1,1,5) # 
v = modbus.read_register(1) #
#v = modbus.read_registers(1,2) #

print("respuesta :")
print(v)

#modbus.write_register(1,27,functioncode=6) #
#modbus.write_register(1,27,functioncode=16) #
#modbus.write_register(1,5.7,numberOfDecimals=1,functioncode=16) #
#modbus.write_registers(1,[27,28])

