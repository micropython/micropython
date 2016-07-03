from unittest import TestCase
import pyb
  
class TestTimers (TestCase):
    testCounter=0

    def test_1(self):
        t0 =  pyb.Timer(0)
        t0.init(prescaler=1000, period=2000)
        
        self.assertGT(1000,t0.prescaler(),"init prescaler")        
        self.assertEqual(2000,t0.period(),"init period")        

    def test_2(self):
        t0 =  pyb.Timer(0)
        t0.init(freq=1000)
        self.assertEqual(1000,t0.freq(),"init freq")

    def test_3(self):
        flagOk=False
        t0 =  pyb.Timer(0)
        try:
            t0.init(freq=2000000) #2Mhz:Invalid
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"init freq invalid")


    def test_4(self):
        t0 =  pyb.Timer(0)
        t0.deinit()
        pyb.delay(10)
        t0.init(freq=100)

        TestTimers.testCounter=0
        t0.callback(self.callbackTest)
        pyb.delay(100)
        t0.deinit()
        self.assertGT(TestTimers.testCounter,8,"callback counter")


    def callbackTest(self,sw):
        TestTimers.testCounter+=1

    def test_5(self):
        t0 =  pyb.Timer(0)
        t0.init(freq=1000)
        flagOk=False
        try:
            t0.callback(99)
            flagOk=True
        except:
            pass
        self.assertEqual(False,flagOk,"callback invalid")


    def test_6(self):
        t0 =  pyb.Timer(0)
        t0.init(freq=1000)
        t0.counter(0)
        pyb.delay(100)
        t0.deinit()
        self.assertGT(t0.counter(),8,"timer counter")              

    def test_7(self):
        t0 =  pyb.Timer(0)
        t0.deinit()
        pyb.delay(10)

        TestTimers.testCounter=0
        t0.interval(10,self.callbackTest)

        pyb.delay(100)
        t0.deinit()
        self.assertGT(TestTimers.testCounter,8,"callback interval")

    def test_8(self):
        t0 =  pyb.Timer(0)
        t0.deinit()
        pyb.delay(10)

        TestTimers.testCounter=0
        t0.timeout(10,self.callbackTest)

        pyb.delay(100)
        t0.deinit()
        self.assertEqual(1,TestTimers.testCounter,"callback timeout")

    def test_9(self):
        t0 =  pyb.Timer(0)
        flagOk=False
        try:
            t0.interval(10,99)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"callback interval invalid")


    def test_10(self):
        t0 =  pyb.Timer(0)
        flagOk=False
        try:
            t0.timeout(10,99)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"callback timeout invalid")


    def test_11(self):
        flagOk=False
        try:
            t0 =  pyb.Timer(99)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"create timer invalid")
