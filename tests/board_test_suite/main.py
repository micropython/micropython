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
`board_test_suite`
====================================================
CircuitPython board hardware test suite

* Author(s): Shawn Hymel
* Date: December 8, 2018

Implementation Notes
--------------------
Run this to test various input/output abilities of a board. Tests the following:

* Onboard LEDs
* GPIO output
* Onboard battery voltage monitor
* SPI
* I2C

You will need the following components:

* Multimeter
* LED
* 1x 330 Ohm resistor
* 2x 4.7k Ohm resistor
* Microchip 25AA040A SPI EEPROM
* Microchip AT24HC04B I2C EEPROM
* Breadboard
* Wires

Copy lib directory to CIRCUITPYTHON drive. Copy the contents of this file to
main.py in root of CIRCUITPYTHON. Open Serial terminal to board and follow
prompts given.
"""

import board

import led_test
import gpio_test
import voltage_monitor_test
import uart_test
import spi_test
import i2c_test

# Constants
UART_TX_PIN_NAME = 'TX'
UART_RX_PIN_NAME = 'RX'
UART_BAUD_RATE = 9600
SPI_MOSI_PIN_NAME = 'MOSI'
SPI_MISO_PIN_NAME = 'MISO'
SPI_SCK_PIN_NAME = 'SCK'
SPI_CS_PIN_NAME = 'D2'
I2C_SDA_PIN_NAME = 'SDA'
I2C_SCL_PIN_NAME = 'SCL'

# Results dictionary
test_results = {}

# Save tested pins
pins_tested = []

# Print welcome message
print()
print("                            ....                                      ")
print("                        #@@%%%%%%&@@/                                 ")
print("                     (&@%%%%%%%%%%%%%@&                               ") 
print("                  .(@&%%%@*    *&%%%%%%@.                             ") 
print("            ,@@&&%%%%%%%%//@%,/ /&%%%%%%@                             ")
print("            %@%%%&%%%%%%%#(@@@&&%%%%%%%%@*                            ")
print("             @&%%&%%%%%%%%%%%%%%%%%%%%%%@/                            ")
print("               &@@&%%%%&&&%%%%%%%%%%%%%%@,                            ")
print("                ,/ &@&&%%%%%%%%%%%%%%%%%@                             ")
print("               ,*        *@&%%%%%%%%%%%%#                             ")
print("               (           @%%%%%%%%%%%@                              ")
print("              ,            @%%%%%%%%%%&@                              ")
print("                          #&%%%%%%%%%%@.                              ")
print("                         #@###%%%%%%%@/                               ")
print("                        (@##(%%%%%%%@%                                ")
print("                       /@###(#%%%%%&@                                 ")
print("                      #@####%%%%%%%@                                  ")
print("                     (@###(%%%%%%%@,                                  ")
print("                    .@##(((#%%%%%&(         .,,.                      ")
print("                   ,@#####%%%%%%%@    ,%@@%%%%%%%&@%                  ")
print("                ,#&@####(%%%%%%%@@@@@&%%%%%%%%%%%###&                 ")
print("               @%%@%####(#%%%%%&@%%%%%%%%%%%%%%##/((@@@@&*            ")
print("              (##@%#####%%%%%%%@(#%%%(/####(/####(%@%%%%%%@/          ")
print("           (@&%@@###(#%%%%%%@&/####(/#####/#&@@&%%%%%%%##@            ")
print("          #@%%%%@#####(#%%%%%%@@@@@@@@@@@@@&%%%%%%%%%%%%#/(@@@@@/     ")
print("          @%(/#@%######%%%%%%%@%%%%%%%%%%%%%%%%%%%%%(/(###@%%%%%%@%   ")
print("         .@@#(#@#####(#%%%%%%&@###//#####/#####/(####/#%@&%%%%%%%%&&  ")
print("        /@%%&@@@(#((((#%%%%%%&@###((#####/#####((##%@@&%%%%%%%%%%%/@. ")
print("       ,@%%%%%%#####%%%%%%%%@@@@&&&&&&&%&@@@@@@&%%%%%%%%%%%%%%%##@,   ")
print("       %%%%%%%%@######(%%%%%%%@&%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#/(#&&  ") 
print("       (@###/(%@##((##(%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%##%###/(&&   ")
print("    ,@@%@%##((#%@#######%%%%%%%%@&%%%%##%%%%##%%%%#/#####((####(@*    ")
print("  *&(,    %@@%##%@#######(%%%%%%%%@#/#####((#####(#####(/#&@&.        ")
print("                 .@###((#%%%%%%%%%&@@###((#####(###%@@&,              ")
print("                   #@#(#######%&@@&* .*#&@@@@@@@%(,                   ")
print("                          .,,,..                                      ")
print()
print("**********************************************************************")
print("*           Welcome to the CircuitPython board test suite!           *")
print("*              Follow the directions to run each test.               *")
print("**********************************************************************")
print()

# List out all the pins available to us
pins = [p for p in dir(board)]
print("All pins found:", end=' ')

# Print pins
for p in pins:
    print(p, end=' ')
print('\n')

# Run LED test
print("@)}---^-----  LED TEST  -----^---{(@")
print()
result = led_test.run_test(pins)
test_results["LED Test"] = result[0]
pins_tested.append(result[1])
print()
print(result[0])
print()

# Run GPIO test
print("@)}---^-----  GPIO TEST  -----^---{(@")
print()
result = gpio_test.run_test(pins)
test_results["GPIO Test"] = result[0]
pins_tested.append(result[1])
print()
print(result[0])
print()

# Run voltage monitor test
print("@)}---^-----  VOLTAGE MONITOR TEST  -----^---{(@")
print()
result = voltage_monitor_test.run_test(pins)
test_results["Voltage Monitor Test"] = result[0]
pins_tested.append(result[1])
print()
print(result[0])
print()

# Run UART test
print("@)}---^-----  UART TEST  -----^---{(@")
print()
result = uart_test.run_test(pins, UART_TX_PIN_NAME, UART_RX_PIN_NAME, UART_BAUD_RATE)
test_results["UART Test"] = result[0]
pins_tested.append(result[1])
print()
print(result[0])
print()

# Run SPI test
print("@)}---^-----  SPI TEST  -----^---{(@")
print()
result = spi_test.run_test( pins,
                            mosi_pin=SPI_MOSI_PIN_NAME, 
                            miso_pin=SPI_MISO_PIN_NAME, 
                            sck_pin=SPI_SCK_PIN_NAME,
                            cs_pin=SPI_CS_PIN_NAME)
test_results["SPI Test"] = result[0]
pins_tested.append(result[1])
print()
print(result[0])
print()

# Run I2C test
print("@)}---^-----  I2C TEST  -----^---{(@")
print()
result = i2c_test.run_test(pins, sda_pin=I2C_SDA_PIN_NAME, scl_pin=I2C_SCL_PIN_NAME)
test_results["I2C Test"] = result[0]
pins_tested.append(result[1])
print()
print(result[0])
print()

# Print out test results
print("@)}---^-----  TEST RESULTS  -----^---{(@")
print()

# Find appropriate spaces for printing test results
num_spaces = 0
for key in test_results:
	if len(key) > num_spaces:
		num_spaces = len(key)

# Print test results
for key in test_results:
	print(key + ":", end=' ')
	for i in range(num_spaces - len(key)):
		print(end=' ')
	print(test_results[key])
print()

# Figure out which pins were tested and not tested
tested = []
for sublist in pins_tested:
	for p in sublist:
		tested.append(p)
not_tested = list(set(pins).difference(set(tested)))

# Print tested pins
print("The following pins were tested:", end=' ')
for p in tested:
	print(p, end=' ')
print('\n')

# Print pins not tested
print("The following pins were NOT tested:", end=' ')
for p in not_tested:
	print(p, end=' ')
print('\n')