from machine import SPI, Pin
from ubinascii import hexlify
import random


class WinbondFlash: #beta class to the SPI Flash in Tiger
    def __init__(self): #constructor
        self.__cs = Pin("SPI_FLASH_CS", mode=Pin.OUT, value=1) #defines the CS pin
        self.__spi = SPI(6) #defines the SPI
        self.__spi.init(baudrate=50_000_000, polarity=0, phase=0, bits=8, firstbit=SPI.MSB) #initializes the SPI


    def fast_read(self, address, n_bytes): #reads the "n_bytes" from "address" in the memory and returns the data read

        cmd = 0x0B
        cmd = cmd.to_bytes(1, 'big')

        address = address.to_bytes(3,'big')

        recv = bytearray(n_bytes)

        dummy_clock = bytearray(1)

        self.__cs.low()  # Select peripheral.
        self.__spi.write(cmd+address+dummy_clock)
        self.__spi.readinto(recv)
        self.__cs.high()

        return recv


    def write_enable(self):#enables the write process in the memory

        cmd = 0x06
        cmd = cmd.to_bytes(1,'big')
        self.__cs.low()  # Select peripheral.
        self.__spi.write(cmd)
        self.__cs.high()


    def page_program(self, address, data, n_bytes):#writes "data" in "address" of the memory

        cmd = 0x02
        cmd = cmd.to_bytes(1, 'big')

        if address > 0xFFFFFF:
            return

        if n_bytes == 256:
            if address & 0xFF != 0:
                return

        address = address.to_bytes(3, 'big')

        self.__cs.low()  # Select peripheral.
        self.__spi.write(cmd + address)

        for i in range(n_bytes):
            self.__spi.write(data[i].to_bytes(1, 'big'))

        self.__cs.high()

    def sector_erase(self, address):#erases a 4k memory sector starting from "address"

        cmd = 0x52
        cmd = cmd.to_bytes(1, 'big')

        address = address.to_bytes(3, 'big')

        self.__cs.low()  # Select peripheral.
        self.__spi.write(cmd + address)
        self.__cs.high()

    def read_unique_id_number(self): #reads the unique number id from the memory

        cmd = 0x4B
        cmd = cmd.to_bytes(1,'little')
        dummy_bytes = bytearray(4)

        recv = bytearray(8)
        self.__cs.low()  # Select peripheral.
        self.__spi.write(cmd+dummy_bytes)
        self.__spi.readinto(recv)
        self.__cs.high()

        return recv

    def read_status_register(self,register=1):

        if register > 3 or register < 1:
            return

        if register == 1 :
            cmd = 0x05
        elif register == 2:
            cmd = 0x35
        else:
            cmd = 0x15

        recv = bytearray(1)

        cmd = cmd.to_bytes(1, 'big')
        self.__cs.low()  # Select peripheral.
        self.__spi.write(cmd)
        self.__spi.readinto(recv)
        self.__cs.high()

        return recv


def print_empty_lines(n):
    for j in range(n):
        print()

def wait_memory_busy():
    stat = 1

    while (stat & 0x1) == 1:
        temp = flash.read_status_register(1)
        stat = int(hexlify(temp).decode('utf-8'))

def prepare_data(length):
    data = []  # data to be written on flash
    sequence = list(range(0, 256, 1))  # creates a sequence from 0 to 255

    for i in range(length):
        data.append(random.choice(sequence))  # picks a random number from the sequence and add it to the data to be written
        sequence.remove(data[i])  # remove the same number from the sequence

    return data

'''
flashAddress = 0x000000 #Flash Address to write/read in order to test the flash memory

flash = WinbondFlash()#flash chip object

readData = flash.fast_read(flashAddress, 256)#data from the previous execution of the program



print("********** PREVIOUS CONTENT INSIDE FLASH MEMORY **********")

print_empty_lines(1)

print(list(readData))#print data written in a previous

print_empty_lines(1)

print("***********************************************************")



flash.write_enable() #enables write operation in flash

flash.sector_erase(flashAddress) # erase 4KB from the memory

wait_memory_busy()#wait until memory is not busy

readData = flash.fast_read(flashAddress, 256) #reads the data in the memory region after erase



print_empty_lines(3)

print("********** MEMORY CONTENT AFTER THE ERASE **********")

print_empty_lines(1)

print(list(readData))

print_empty_lines(1)

print("*****************************************************")

print_empty_lines(3)



size = random.randint(1,256) #picks a random size to be written
print(("Size to be written: " + str(size)))



writeData = prepare_data(size)#initialize the data to be written

flash.write_enable() #enables write operation in flash

flash.page_program(flashAddress,writeData, len(writeData))#writes the data previously prepare



print_empty_lines(3)

print("********** FLASH MEMORY'S UNIQUE ID **********")

print_empty_lines(1)

wait_memory_busy()#wait until memory is not busy

flashId = hexlify(flash.read_unique_id_number()).decode('utf-8')#reads the unique flash id

print(flashId)

print_empty_lines(1)

print("*****************************************************")

print_empty_lines(3)



print("********** NEW CONTENT OF THE FLASH MEMORY **********")

print_empty_lines(1)

readData = flash.fast_read(flashAddress, size)#reads the data that was just written

print(list(readData))

print_empty_lines(1)

print("*****************************************************")

'''