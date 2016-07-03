from unittest import TestCase
from testing_TestLeds import TestLeds
from testing_TestSwitches import TestSwitches
from testing_TestUart import TestUart
from testing_TestEEPROM import TestEEPROM
from testing_TestDAC import TestDAC
from testing_TestADC import TestADC
from testing_TestGPIO import TestGPIO
from testing_TestRS485 import TestRS485
from testing_TestTimers import TestTimers


class MainTest (object):

    def __init__(self):
        pass

    def run(self):
        print("Running python tests")

        print("LEDs Tests")
        TestCase.run(TestLeds())
        print("____________________")

        print("Switches Tests")
        TestCase.run(TestSwitches())
        print("____________________")

        print("UART Tests")
        TestCase.run(TestUart())
        print("____________________")

        print("EEPROM Tests")
        TestCase.run(TestEEPROM())
        print("____________________")

        print("DAC Tests")
        TestCase.run(TestDAC())
        print("____________________")


        print("ADC Tests")
        TestCase.run(TestADC())
        print("____________________")

        print("GPIOs Tests")
        TestCase.run(TestGPIO())
        print("____________________")

        print("RS485 Tests")
        TestCase.run(TestRS485())
        print("____________________")

        print("Timers Tests")
        TestCase.run(TestTimers())
        print("____________________")



        print("Statistics:")
        TestCase.printStatistics()
