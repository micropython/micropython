from unittest import TestCase
import pyb
  
class TestDAC (TestCase):

    def test_1(self):
        dac = pyb.DAC(1)
        r = dac.write(512)
        self.assertEqual(True,r,"DAC write value")        

    def test_2(self):
        dac = pyb.DAC(1)
        r = dac.write(2000)
        self.assertEqual(False,r,"DAC write invalid value")

    def test_3(self):
        dac = pyb.DAC(1)

        buf = bytearray(8)
        buf[0]=0
        buf[1]=0

        buf[2]=8
        buf[3]=0

        buf[4]=16
        buf[5]=0

        buf[6]=2
        buf[7]=0

        r = dac.write_timed(buf, 10000*(int(len(buf)/2)), mode=pyb.DAC.CIRCULAR)

        self.assertEqual(8,r,"DAC with DMA")



