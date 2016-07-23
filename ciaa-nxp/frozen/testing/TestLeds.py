from unittest import TestCase
import pyb
  
class TestLeds (TestCase):

    def test_1(self):
        led1 = pyb.LED(1)
        led1.on()
        self.assertEqual(1,led1.value(),"LED State ON")        
        led1.off()
        self.assertEqual(0,led1.value(),"LED State OFF")


    def test_2(self):
        ledRed = pyb.LED(4)
        ledRed.intensity(8)
        self.assertEqual(8,ledRed.intensity(),"LED Intensity 8")

        ledGreen = pyb.LED(5)
        ledGreen.intensity(5)
        self.assertEqual(5,ledGreen.intensity(),"LED Intensity 5")

        ledBlue = pyb.LED(6)
        ledBlue.intensity(9)
        self.assertEqual(9,ledBlue.intensity(),"LED Intensity 9")


    def test_3(self):
        led1 = pyb.LED(1)
        led2 = pyb.LED(2)
        led3 = pyb.LED(3)

        led1.on()
        led2.off()
        led3.off()

        led1.toggle()
        self.assertEqual(0,led1.value(),"LED Toogle OFF")

        led2.toggle()
        self.assertEqual(1,led2.value(),"LED Toogle ON 1")

        led3.toggle()
        self.assertEqual(1,led3.value(),"LED Toogle ON 2")

