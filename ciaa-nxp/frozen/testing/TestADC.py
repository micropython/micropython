from unittest import TestCase
import pyb

class TestADC (TestCase):

    def test_1(self):
        channel1 = pyb.ADC(1)
        self.assertNotEqual(channel1,None,"Enable CH1")        


    def test_2(self):
        channel2 = pyb.ADC(2)
        self.assertNotEqual(channel2,None,"Enable CH2")

    def test_3(self):
        channel3 = pyb.ADC(3)
        self.assertNotEqual(channel3,None,"Enable CH3")

    def test_4(self):
        flagOk=False
        try :
            channel = pyb.ADC(99)
            flagOk=True
        except:
            pass

        self.assertEqual(flagOk,False,"Enable invalid CH")

    def test_5(self):
        channel = pyb.ADC(1)
        v = channel.read()
        self.assertEqual(v,0,"Read CHN 1")

    def test_6(self):
        channel = pyb.ADC(2)
        v = channel.read()
        self.assertEqual(v,0,"Read CHN 2")

    def test_7(self):
        channel = pyb.ADC(3)
        v = channel.read()
        self.assertEqual(v,0,"Read CHN 3")


