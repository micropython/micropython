from unittest import TestCase
import pyb
  
class TestRS485 (TestCase):

    def __init__(self):
        self.uart = None

    def setUp(self):
        pyb.delay(250)
        if self.uart!=None:
            while self.uart.any():
                self.uart.readall()
        pyb.delay(250)

    def test_1(self):
        flagOk=False
        try:
            uart = pyb.UART(0)
            flagOk=True
        except:
            pass
        self.assertEqual(True,flagOk,"Create Object OK")

    def test_2(self):
        flagOk=False
        try:
            uart = pyb.UART(99)
            flagOk=True
        except:
            pass
        self.assertEqual(False,flagOk,"Create Object Invalid uart number")


    def test_3(self):
        flagOk=False
        try:
            uart = pyb.UART(0)
            uart.init(9600,bits=99, parity=None, stop=1)
            flagOk=True
        except:
            pass
        self.assertEqual(False,flagOk,"Create Object Invalid bits")

    def test_4(self):
        flagOk=False
        try:
            uart = pyb.UART(0)
            uart.init(9600,bits=8, parity=None, stop=99)
            flagOk=True
        except:
            pass
        self.assertEqual(False,flagOk,"Create Object Invalid Stop bits")


    def test_5(self):
        self.uart = pyb.UART(0)
        self.uart.init(9600,bits=8, parity=None, stop=1)
        r = self.uart.write("Test string")
        self.assertEqual(11,r,"Bytes sent")


    def test_6(self):
        self.uart = pyb.UART(0)
        self.uart.init(9600,bits=8, parity=None, stop=1)

        self.uart.write("A")
        pyb.delay(150)
        any = self.uart.any()
        self.assertEqual(True,any,"any() method in char mode")

        data = self.uart.readall()
        self.assertEqual(0x41,data[0],"Received character")

        self.uart=None

    def test_7(self):
        self.uart = pyb.UART(0)
        self.uart.init(9600,bits=8, parity=None, stop=1,timeout=100, timeout_char=0, read_buf_len=32,packet_mode=True)

        self.uart.write("Test string")

        to  = 400
        while to > 0:
            if self.uart.any():
                break
            pyb.delay(1)
            to-=1

        self.assertNotEqual(0,to,"any() method in packet mode")

        data = self.uart.readall()
        self.assertEqual(11,len(data),"Amount of received bytes packet mode")

        self.assertEqual(bytearray("Test string"),data,"Content comparison")

        






