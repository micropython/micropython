from unittest import TestCase
import pyb

class TestGPIO (TestCase):

    flagInCallback=False

    def test_1(self):
        flagOk=False
        try:
            p = pyb.Pin(0) #GPIO0
            p.init(pyb.Pin.IN,pyb.Pin.PULL_NONE)
            flagOk=True
        except:
            pass

        self.assertEqual(flagOk,True,"Enable GPIO0 IN")        


    def test_2(self):
        flagOk=False
        try:
            p = pyb.Pin(8) #GPIO8
            p.init(pyb.Pin.IN,pyb.Pin.PULL_NONE)
            flagOk=True
        except:
            pass

        self.assertEqual(flagOk,True,"Enable GPIO8 IN")

    def test_3(self):
        flagOk=False
        try:
            p = pyb.Pin(9) #GPIO9 - invalid
            p.init(pyb.Pin.IN,pyb.Pin.PULL_NONE)
            flagOk=True
        except:
            pass

        self.assertEqual(flagOk,False,"Enable invalid GPIO9 IN")

    def test_4(self):
        flagOk=False
        try:
            p = pyb.Pin(0) #GPIO0
            p.init(99,pyb.Pin.PULL_NONE) #invalid mode
            flagOk=True
        except:
            pass

        self.assertEqual(flagOk,False,"Enable invalid mode GPIO0 IN")

    def test_5(self):
        flagOk=False
        try:
            p = pyb.Pin(0) #GPIO0
            p.init(pyb.Pin.IN,99) #invalid pullup mode
            flagOk=True
        except:
            pass

        self.assertEqual(flagOk,False,"Enable invalid pullup GPIO0 IN")

    def test_6(self):
        p = pyb.Pin(0)
        p.init(pyb.Pin.IN,pyb.Pin.PULL_UP)
        pyb.delay(10)
        v = p.value()
        self.assertEqual(v,1,"Pull Up")

    def test_7(self):
        p = pyb.Pin(0)
        p.init(pyb.Pin.IN,pyb.Pin.PULL_DOWN)
        pyb.delay(10)
        v = p.value()
        self.assertEqual(v,0,"Pull Down")

    def __writeSetup(self):
        self.p7 = pyb.Pin(7)
        self.p7.init(pyb.Pin.OUT_PP,pyb.Pin.PULL_NONE)
        pyb.delay(1)
        self.p8 = pyb.Pin(8)
        self.p8.init(pyb.Pin.IN,pyb.Pin.PULL_NONE)
        pyb.delay(1)


    def test_8(self):
        self.__writeSetup()

        self.p7.low()
        pyb.delay(10)
        v = self.p8.value()
        self.assertEqual(v,0,"Write Low test")        

    def test_9(self):
        self.__writeSetup()

        self.p7.high()
        pyb.delay(10)
        v = self.p8.value()
        self.assertEqual(v,1,"Write High test")


    def callbackTest(self,line):
        TestGPIO.flagInCallback=True

    def test_10(self):
        self.__writeSetup()
        
        int = pyb.ExtInt(self.p8,pyb.ExtInt.IRQ_RISING,pyb.Pin.PULL_NONE,self.callbackTest)
        int.disable()
        pyb.delay(10)
        self.p7.low()
        int.enable()

        # generate interrupt
        TestGPIO.flagInCallback=False
        self.p7.high()
        pyb.delay(10)   
        self.assertEqual(TestGPIO.flagInCallback,True,"Rising edge interrupt")


    def test_11(self):
        flagOk=False
        try:
            int = pyb.ExtInt(None,pyb.ExtInt.IRQ_RISING,pyb.Pin.PULL_NONE,self.callbackTest)
            flagOk=True
        except:
            pass
        self.assertEqual(flagOk,False,"Interrupt. Invalid Pin obj")


    def test_12(self):
        flagOk=False
        try:
            int = pyb.ExtInt(Self.p8,99,pyb.Pin.PULL_NONE,self.callbackTest)
            flagOk=True
        except:
            pass
        self.assertEqual(flagOk,False,"Interrupt. Invalid edge")


    def test_13(self):
        flagOk=False
        try:
            int = pyb.ExtInt(Self.p8,pyb.ExtInt.IRQ_RISING,99,self.callbackTest)
            flagOk=True
        except:
            pass
        self.assertEqual(flagOk,False,"Interrupt. Invalid pull")

