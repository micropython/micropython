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
`i2c_test`
====================================================
I2C Test Module

* Author(s): Shawn Hymel
* Date: December 8, 2018

Implementation Notes
--------------------
Performs random writes and reads to I2C EEPROM.

Requires Microchip AT24HC04B I2C EEPROM.

Run this script as its own main.py to individually run the test, or compile 
with mpy-cross and call from separate test script.
"""

import board
import busio
import random
import time

# Constants
SDA_PIN_NAME = 'SDA'
SCL_PIN_NAME = 'SCL'
NUM_I2C_TESTS = 10          # Number of times to write and read EEPROM values
EEPROM_I2C_MAX_ADDR = 255   # Self-imposed max memory address

# Microchip AT24HC04B EEPROM I2C address
EEPROM_I2C_ADDR = 0x50

# Test result strings
PASS = "PASS"
FAIL = "FAIL"
NA = "N/A"

# Open comms to I2C EEPROM by trying a write to memory address
def _eeprom_i2c_wait(i2c, i2c_addr, mem_addr, timeout = 1.0):
    
    # Try to access the I2C EEPROM (it becomes unresonsive during a write)
    timestamp = time.monotonic()
    while time.monotonic() < timestamp + timeout:
        try:
            i2c.writeto(i2c_addr, bytearray([mem_addr]), end=1, stop=False)
            return True
        except:
            pass
            
    return False

# Write to address. Returns status (True for successful write, False otherwise)
def _eeprom_i2c_write_byte(i2c, i2c_addr, mem_addr, mem_data, timeout = 1.0):
    
    # Make sure address is only one byte:
    if mem_addr > 255:
        return False
        
    # Make sure data is only one byte:
    if mem_data > 255:
        return False
        
    # Write data to memory at given address
    try:
        i2c.writeto(i2c_addr, bytearray([mem_addr, mem_data]))
    except:
        return False
    
    return True

# Read from address. Returns tuple [status, result]
def _eeprom_i2c_read_byte(i2c, i2c_addr, mem_addr, timeout = 1.0):
    
    # Make sure address is only one byte:
    if mem_addr > 255:
        return False, bytearray()
   
    # Try writing to address (EEPROM is unresponsive while writing)
    if _eeprom_i2c_wait(i2c, i2c_addr, mem_addr, timeout) == False:
		return False, bytearray()
	 
    # Finish the read
    buf = bytearray(1)
    i2c.readfrom_into(i2c_addr, buf)
    
    return True, buf

def run_test(pins, sda_pin=SDA_PIN_NAME, scl_pin=SCL_PIN_NAME):
    
	# Write values to I2C EEPROM and verify the values match
	if list(set(pins).intersection(set([sda_pin, scl_pin]))):

		# Tell user to connect EEPROM chip
		print("Connect a Microchip AT24HC04B EEPROM I2C chip. " +
				"Press enter to continue.")
		input()

		# Set up I2C
		i2c = busio.I2C(getattr(board, scl_pin), getattr(board, sda_pin))

		# Wait for I2C lock
		while not i2c.try_lock():
			pass

		# Pick a random address, write to it, read from it, and see if they match
		pass_test = True
		for i in range(NUM_I2C_TESTS):

			# Randomly pick an address and a data value (one byte)
			mem_addr = random.randint(0, EEPROM_I2C_MAX_ADDR)
			mem_data = random.randint(0, 255)
			print("Address:\t" + hex(mem_addr))
			print("Writing:\t" + hex(mem_data))

			# Try writing this random value to the random address
			result = _eeprom_i2c_write_byte(i2c, EEPROM_I2C_ADDR, mem_addr, mem_data)
			if result == False:
				print("FAIL: I2C could not communicate")
				pass_test = False
				break

			# Try reading the written value back from EEPROM
			result = _eeprom_i2c_read_byte(i2c, EEPROM_I2C_ADDR, mem_addr)
			print("Read:\t\t" + hex(result[1][0]))
			print()
			if result[0] == False:
				print("FAIL: I2C could not communicate")
				pass_test = False
				break

			# Compare the read value to the original value
			if result[1][0] != mem_data:
				print("FAIL: Data does not match")
				pass_test = False
				break

		# Release I2C pins
		i2c.deinit()

		# Store results
		if pass_test:
			return PASS, [sda_pin, scl_pin]
		else:
			return FAIL, [sda_pin, scl_pin]
	
	else:
		print("No I2C pins found")
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