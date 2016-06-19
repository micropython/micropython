from unittest import TestCase
from testing_TestLeds import TestLeds
from testing_TestSwitches import TestSwitches
from testing_TestUart import TestUart
from testing_TestEEPROM import TestEEPROM


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


        print("Statistics:")
        TestCase.printStatistics()
