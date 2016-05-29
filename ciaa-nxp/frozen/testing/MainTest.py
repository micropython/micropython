from unittest import TestCase
from testing_TestLeds import TestLeds

class MainTest (object):

    def __init__(self):
        pass

    def run(self):
        print("Running python tests")

        print("LEDs Tests")
        TestCase.run(TestLeds())
        print("____________________")



        print("Statistics:")
        TestCase.printStatistics()
