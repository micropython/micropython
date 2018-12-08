# The MIT License (MIT)
#
# Copyright (c) 2018 Shawn Hymel for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
`spi_test`
====================================================
SPI Test Module

* Author(s): Shawn Hymel
* Date: December 8, 2018

Implementation Notes
--------------------
Performs random writes and reads to SPI EEPROM.

Requires Microchip 25AA040A SPI EEPROM.

Run this script as its own main.py to individually run the test, or compile 
with mpy-cross and call from separate test script.
"""

import board
import digitalio
import busio
import random
import time

# Constants
MOSI_PIN_NAME = 'MOSI'
MISO_PIN_NAME = 'MISO'
SCK_PIN_NAME = 'SCK'
CS_PIN_NAME = 'D2'
BAUD_RATE = 100000          # Bits per second
NUM_SPI_TESTS = 10          # Number of times to write and read EEPROM values

# Microchip 25AA040A EEPROM SPI commands and bits
EEPROM_SPI_WRSR = 0x01
EEPROM_SPI_WRITE = 0x02
EEPROM_SPI_READ = 0x03
EEPROM_SPI_WRDI = 0x04
EEPROM_SPI_RDSR = 0x05
EEPROM_SPI_WREN = 0x06
EEPROM_SPI_WIP_BIT = 0
EEPROM_SPI_MAX_ADDR = 255   # Self-imposed max memory address
EEPROM_I2C_MAX_ADDR = 255   # Self-imposed max memory address

# Test result strings
PASS = "PASS"
FAIL = "FAIL"
NA = "N/A"

# Wait for WIP bit to go low
def _eeprom_spi_wait(spi, cs, timeout = 1.0):
    
    # Continually read from STATUS register
    timestamp = time.monotonic()
    while time.monotonic() < timestamp + timeout:
        
        # Perfrom RDSR operation
        cs.value = False
        result = bytearray(1)
        spi.write(bytearray([EEPROM_SPI_RDSR]))
        spi.readinto(result)
        cs.value = True
        
        # Mask out and compare WIP bit
        if (result[0] & (1 << EEPROM_SPI_WIP_BIT)) == 0:
            return True
            
    return False
    
# Write to address. Returns status (True for successful write, False otherwise)
def _eeprom_spi_write_byte(spi, cs, address, data, timeout = 1.0):
    
    # Make sure address is only one byte:
    if address > 255:
        return False
        
    # Make sure data is only one byte:
    if data > 255:
        return False
    
    # Wait for WIP to be low
    if _eeprom_spi_wait(spi, cs, timeout) == False:
        return False
        
    # Enable writing
    cs.value = False
    spi.write(bytearray([EEPROM_SPI_WREN]))
    cs.value = True
    
    # Write to address
    cs.value = False
    spi.write(bytearray([EEPROM_SPI_WRITE, address, data]))
    cs.value = True
    
    return True

# Read from address. Returns tuple [status, result]
def _eeprom_spi_read_byte(spi, cs, address, timeout = 1.0):
    
    # Make sure address is only one byte:
    if address > 255:
        return False, bytearray()
    
    # Wait for WIP to be low
    if _eeprom_spi_wait(spi, cs, timeout) == False:
        return False, bytearray()
    
    # Read byte from address
    cs.value = False
    result = bytearray(1)
    spi.write(bytearray([EEPROM_SPI_READ, address]))
    spi.readinto(result)
    cs.value = True
    
    return True, result

def run_test(   pins,
                mosi_pin=MOSI_PIN_NAME, 
                miso_pin=MISO_PIN_NAME, 
                sck_pin=SCK_PIN_NAME,
                cs_pin=CS_PIN_NAME):
    
    # Write values to SPI EEPROM and verify the values match
    if list(set(pins).intersection(set([mosi_pin, miso_pin, sck_pin]))):

        # Tell user to connect EEPROM chip
        print("Connect a Microchip 25AA040A EEPROM SPI chip.")
        print("Connect " + cs_pin + " to the CS pin on the 25AA040.")
        print("Press enter to continue.")
        input()

        # Configure CS pin
        cs = digitalio.DigitalInOut(getattr(board, cs_pin))
        cs.direction = digitalio.Direction.OUTPUT
        cs.value = True

        # Set up SPI
        spi = busio.SPI(getattr(board, sck_pin), 
						MOSI=getattr(board, mosi_pin), 
						MISO=getattr(board, miso_pin))

        # Wait for SPI lock
        while not spi.try_lock():
            pass
        spi.configure(baudrate=BAUD_RATE, phase=0, polarity=0)

        # Pick a random address, write to it, read from it, and see if they match
        pass_test = True
        for i in range(NUM_SPI_TESTS):
            
            # Randomly pick an address and a data value (one byte)
            mem_addr = random.randint(0, EEPROM_SPI_MAX_ADDR)
            mem_data = random.randint(0, 255)
            print("Address:\t" + hex(mem_addr))
            print("Writing:\t" + hex(mem_data))
            
            # Try writing this random value to the random address
            result = _eeprom_spi_write_byte(spi, cs, mem_addr, mem_data)
            if result == False:
                print("FAIL: SPI could not communicate")
                pass_test = False
                break
                
            # Try reading the written value back from EEPRom
            result = _eeprom_spi_read_byte(spi, cs, mem_addr)
            print("Read:\t\t" + hex(result[1][0]))
            print()
            if result[0] == False:
                print("FAIL: SPI could not communicate")
                pass_test = False
                break
                
            # Compare the read value to the original value
            if result[1][0] != mem_data:
                print("FAIL: Data does not match")
                pass_test = False
                break
                
        # Release SPI pins
        spi.deinit()
        
        # Return results
        if pass_test:
            return PASS, [mosi_pin, miso_pin, sck_pin]
        else:
            return FAIL, [mosi_pin, miso_pin, sck_pin]
            
    else:
        print("No SPI pins found")
        return NA, []

def _main():
    
    # List out all the pins available to us
    pins = [p for p in dir(board)]
    print()
    print("All pins found:", end=' ')
    
    # Print pins
    for p in pins:
        print(p, end=' ')
    print('\n')

    # Run test
    result = run_test(pins)
    print()
    print(result[0])
    print("Pins tested: " + str(result[1]))

# Execute only if run as main.py or code.py
if __name__ == "__main__":
    _main()