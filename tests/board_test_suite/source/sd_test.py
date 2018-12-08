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
`sd_test`
====================================================
SD Test Module

* Author(s): Shawn Hymel
* Date: December 8, 2018

Implementation Notes
--------------------
Performs random writes and reads to SD card over SPI.

Requires SD card.

Requires adafruit_sdcard.mpy and adafruit_bus_device modules.

Run this script as its own main.py to individually run the test, or compile 
with mpy-cross and call from separate test script.
"""

import adafruit_sdcard
import board
import busio
import digitalio
import storage
import random

# Constants
MOSI_PIN_NAME = 'SD_MOSI'
MISO_PIN_NAME = 'SD_MISO'
SCK_PIN_NAME = 'SD_SCK'
CS_PIN_NAME = 'SD_CS'
FILENAME = "test.txt"       # File that will be written to
BAUD_RATE = 100000          # Bits per second
NUM_UART_BYTES = 40         # Number of bytes to transmit over UART
ASCII_MIN = 0x21            # '!' Lowest ASCII char in random range (inclusive)
ASCII_MAX = 0x7E            # '~' Highest ASCII char in random range (inclusive)

# Test result strings
PASS = "PASS"
FAIL = "FAIL"
NA = "N/A"

def run_test(   pins,
                mosi_pin=MOSI_PIN_NAME, 
                miso_pin=MISO_PIN_NAME, 
                sck_pin=SCK_PIN_NAME,
                cs_pin=CS_PIN_NAME,
                filename=FILENAME):
                
    # Write characters to file on SD card and verify they were written
    if list(set(pins).intersection(set([mosi_pin, miso_pin, sck_pin]))):
        
        # Tell user to connect SD card
        print("Insert SD card into holder and connect SPI lines to holder.")
        print("Connect " + cs_pin + " to the CS (CD/DAT3) pin on the SD " +
              "card holder.")
        print("WARNING: " + filename + " will be created or overwritten.")
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
        
        # Try to connect to the card and mount the filesystem
        try:
            sdcard = adafruit_sdcard.SDCard(spi, cs)
            vfs = storage.VfsFat(sdcard)
            storage.mount(vfs, "/sd")
        except:
            print("Could not mount SD card")
            return FAIL, [mosi_pin, miso_pin, sck_pin]
        
        # Generate test string
        test_str = ""
        for i in range(NUM_UART_BYTES):
            test_str += chr(random.randint(ASCII_MIN, ASCII_MAX))
        
        # Write test string to a text file on the card
        try:
            with open("/sd/" + filename, "w") as f:
                print("Writing:\t" + test_str)
                f.write(test_str)
        except:
            print("Could not write to SD card")
            return FAIL, [mosi_pin, miso_pin, sck_pin]

        # Read from test file on the card
        read_str = ""
        try:
            with open("/sd/" + filename, "r") as f:
                lines = f.readlines()
                for line in lines:
                    read_str += line
            print("Read:\t\t" + read_str)
        except:
            print("Could not read from SD card")
            return FAIL, [mosi_pin, miso_pin, sck_pin]
        
        # Release SPI
        spi.deinit()
        
        # Compare strings
        if read_str == test_str:
            return PASS, [mosi_pin, miso_pin, sck_pin]
        else:
            return FAIL, [mosi_pin, miso_pin, sck_pin]
    
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