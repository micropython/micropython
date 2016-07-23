from unittest import TestCase
import pyb
  
class TestSwitches (TestCase):
    flagPressed=False

    def test_1(self):
        sw1 =  pyb.Switch(1)
        self.assertEqual(False,sw1.switch(),"SW1 idle State")        

    def test_2(self):
        sw2 =  pyb.Switch(2)
        self.assertEqual(False,sw2.switch(),"SW2 idle State")        

    def test_3(self):
        sw3 =  pyb.Switch(3)
        self.assertEqual(False,sw3.switch(),"SW3 idle State")        

    def test_4(self):
        sw4 =  pyb.Switch(4)
        self.assertEqual(False,sw4.switch(),"SW4 idle State")        


    def test_5(self):
        flagInvalid=True
        try:
            sw =  pyb.Switch(99)
            flagInvalid=False
        except:
            pass

        self.assertEqual(True,flagInvalid,"Invalid SW")

    def callbackTest(self,sw):
        TestSwitches.flagPressed=True


    def test_6(self):
        sw1 =  pyb.Switch(1)
        sw1.callback(self.callbackTest)
        print("Please Press SW1 for checking callback.")
        to=5
        while TestSwitches.flagPressed==False and to>0:
            pyb.delay(1000)
            to-=1
        self.assertEqual(True,TestSwitches.flagPressed,"SW callback")







