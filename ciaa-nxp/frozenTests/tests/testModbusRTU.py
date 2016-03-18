import pyb
import ModBus

class UartMock:
    def write(self,data):
        print("envio:")
        for b in data:
            print(hex(b))

    def read(self,numberOfBytes):
        print("recibo:")
        b = bytearray()

        b.append(0x55)
        b.append(0x03) #fnc 3 con 1 registro de respuesta
        b.append(0x02) # byte count
        b.append(0x11)
        b.append(0x22)
        b.append(0x05)
        b.append(0xC1)
        
        
        print(b)
        return b
        
    def get_baudrate(self):
        return 9600
    def any(self):
        return True
        
uart = UartMock()


modbus = ModBus.Instrument(uart,0x55)

print(modbus)
'''
print("hago read bit")
val = modbus.read_bit(1,2) #OK
print("respuesta :")
print(val)
'''
#modbus.write_bit(1,1,5) # OK
v = modbus.read_register(1) #OK0	0 0
#v = modbus.read_registers(1,2) # OK

print("respuesta :")
print(v)

#modbus.write_register(1,27,functioncode=6) #OK
#modbus.write_register(1,27,functioncode=16) #OK
#modbus.write_register(1,5.7,numberOfDecimals=1,functioncode=16) #
#modbus.write_registers(1,[27,28])

