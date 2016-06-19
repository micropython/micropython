from unittest import TestCase
import pyb
  
class TestEEPROM (TestCase):

    def test_1(self):
        eeprom = pyb.EEPROM()
        eeprom.write_byte(0x0000,0x55)
        val = eeprom.read_byte(0x0000)
        self.assertEqual(0x55,val,"EEPROM addr 0x0000")        


    def test_2(self):
        eeprom = pyb.EEPROM()
        eeprom.write_byte(0x2000,0xC3)
        val = eeprom.read_byte(0x2000)
        self.assertEqual(0xC3,val,"EEPROM addr 0x2000")


    def test_3(self):
        eeprom = pyb.EEPROM()
        eeprom.write_byte(0x3FFF,0x11)
        val = eeprom.read_byte(0x3FFF)
        self.assertEqual(0x11,val,"EEPROM addr 0x3FFF")


    def test_4(self):
        flagOk=False
        try:
            eeprom = pyb.EEPROM()
            eeprom.write_byte(0xFFFF,0xFF)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"EEPROM invalid addr write")


    def test_5(self):
        flagOk=False
        try:
            eeprom = pyb.EEPROM()
            eeprom.read_byte(0xFFFF)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"EEPROM invalid addr read")


    def test_6(self):
        eeprom = pyb.EEPROM()
        eeprom.write_int(0x2000,0xC355)
        val = eeprom.read_int(0x2000)
        self.assertEqual(0xC355,val,"EEPROM int")

    def test_7(self):
        flagOk=False
        try:
            eeprom = pyb.EEPROM()
            eeprom.write_int(0xFFFF,0xFFFF)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"EEPROM int. invalid addr write")


    def test_8(self):
        flagOk=False
        try:
            eeprom = pyb.EEPROM()
            eeprom.read_int(0xFFFF)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"EEPROM int. invalid addr read")


    def test_9(self):
        eeprom = pyb.EEPROM()
        eeprom.write_float(0x2000,3.14)
        val = eeprom.read_float(0x2000)
        self.assertEqual(3.14,val,"EEPROM float")

    def test_10(self):
        flagOk=False
        try:
            eeprom = pyb.EEPROM()
            eeprom.write_float(0xFFFF,3.14)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"EEPROM float. invalid addr write")


    def test_11(self):
        flagOk=False
        try:
            eeprom = pyb.EEPROM()
            eeprom.read_float(0xFFFF)
            flagOk=True
        except:
            pass

        self.assertEqual(False,flagOk,"EEPROM float. invalid addr read")

    def test_12(self):
        eeprom = pyb.EEPROM()
        eeprom.write("Test string")
        data = eeprom.readall()
        self.assertEqual("Test string",data,"EEPROM string")
