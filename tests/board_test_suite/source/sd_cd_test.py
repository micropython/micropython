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
`sd_cd_test`
====================================================
SD CD Test Module

* Author(s): Shawn Hymel
* Date: December 8, 2018

Implementation Notes
--------------------
Reports the output of an SD card's chip detect (CD) pin.

Requires SD card.

Run this script as its own main.py to individually run the test, or compile 
with mpy-cross and call from separate test script.
"""

import board
import digitalio

# Constants
SD_CD_PIN_NAME = 'SD_CD'

# Test result strings
PASS = "PASS"
FAIL = "FAIL"
NA = "N/A"

def run_test(pins, cd_pin=SD_CD_PIN_NAME):
    
    # Ask user to insert and remove SD card
    if list(set(pins).intersection(set([cd_pin]))):
        
        # Configure CD pin as input with pullup
        cd = digitalio.DigitalInOut(getattr(board, cd_pin))
        cd.direction = digitalio.Direction.INPUT
        cd.pull = digitalio.Pull.UP
    
        # Tell user to insert SD card
        print("Connect " + cd_pin + " to CD pin on SD card holder.")
        print("Insert SD card into holder.")
        print("Press enter to continue.")
        input()
        
        # Make sure we see that the pin is low
        if cd.value == True:
            print("Error: Card not detected")
            return FAIL, [cd_pin]
        
        # Tell user to remove SD card
        print("Card detected. Remove card and press enter to continue.")
        input()
        
        # Make sure we see that the pin is high
        if cd.value == False:
            print("Error: Card detected")
            return FAIL, [cd_pin]
        
        # Test passed
        print("Card removed")
        return PASS, [cd_pin]
        
    else:
        print("No CD pin found")
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