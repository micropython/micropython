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
`uart_test`
====================================================
UART Test Module

* Author(s): Shawn Hymel
* Date: December 8, 2018

Implementation Notes
--------------------
Performs random writes and reads across UART.

You will need to connect a loopback wire from TX to RX on your board.

Run this script as its own main.py to individually run the test, or compile 
with mpy-cross and call from separate test script.
"""

import board
import busio
import random

# Constants
TX_PIN_NAME = 'TX'
RX_PIN_NAME = 'RX'
BAUD_RATE = 9600
NUM_UART_BYTES = 40         # Number of bytes to transmit over UART
ASCII_MIN = 0x21            # '!' Lowest ASCII char in random range (inclusive)
ASCII_MAX = 0x7E            # '~' Highest ASCII char in random range (inclusive)

# Test result strings
PASS = "PASS"
FAIL = "FAIL"
NA = "N/A"

def run_test(pins, tx_pin=TX_PIN_NAME, rx_pin=RX_PIN_NAME, baud_rate=BAUD_RATE):
    
    # Echo some values over the UART
    if list(set(pins).intersection(set([tx_pin, rx_pin]))):
        
        # Tell user to create loopback connection
        print("Connect a wire from TX to RX. Press enter to continue.")
        input()
        
        # Initialize UART
        uart = busio.UART(getattr(board, tx_pin), 
                            getattr(board, rx_pin), 
                            baudrate=baud_rate)
        uart.reset_input_buffer()
        
        # Generate test string
        test_str = ""
        for i in range(NUM_UART_BYTES):
            test_str += chr(random.randint(ASCII_MIN, ASCII_MAX))
        
        # Transmit test string
        uart.write(test_str)
        print("Transmitting:\t" + test_str)
        
        # Wait for received string
        data = uart.read(len(test_str))
        recv_str = ''
        if data is not None:
            recv_str = ''.join([chr(b) for b in data])
            print("Received:\t" + recv_str)
            
        # Release UART pins
        uart.deinit()
        
        # Compare strings
        if recv_str == test_str:
            return PASS, [tx_pin, rx_pin]
        else:
            return FAIL, [tx_pin, rx_pin]
            
    else:
        print("No UART pins found")
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