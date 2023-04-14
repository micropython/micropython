from machine import I2C

I2C_BM8563_DEFAULT_ADDRESS = 0x51


class I2C_BM8563_TimeTypeDef:
    def __init__(self, hours, minutes, seconds):
        self.hours = hours
        self.minutes = minutes
        self.seconds = seconds


class I2C_BM8563_DateTypeDef:
    def __init__(self, weekDay, month, date, year):
        self.weekDay = weekDay
        self.month = month
        self.date = date
        self.year = year


class rtc_BM8563:
    def __init__(self, i2cPort, deviceAddress=I2C_BM8563_DEFAULT_ADDRESS):
        self._deviceAddress = deviceAddress
        self._i2cPort = i2cPort

    def begin(self):
        self._i2cPort.writeto(self._deviceAddress, bytearray([0x00, 0x00, 0x00]))

    def get_volt_low(self):
        data = self.ReadReg(0x02)
        return data & 0x80

    def bcd2_to_byte(self, value):
        tmp = ((value & 0xF0) >> 4) * 10
        return tmp + (value & 0x0F)

    def byte_to_bcd2(self, value):
        bcdhigh = 0
        while value >= 10:
            bcdhigh += 1
            value -= 10
        return (bcdhigh << 4) | value

    def get_time(self):
        i2c_bm8563_time_struct:I2C_BM8563_TimeTypeDef = I2C_BM8563_TimeTypeDef(0,0,0)
        buf = bytearray(3)
        self._i2cPort.writeto(self._deviceAddress, bytearray([0x02]))
        self._i2cPort.readfrom_into(self._deviceAddress, buf)

        i2c_bm8563_time_struct.seconds = self.bcd2_to_byte(buf[0] & 0x7f)
        i2c_bm8563_time_struct.minutes = self.bcd2_to_byte(buf[1] & 0x7f)
        i2c_bm8563_time_struct.hours = self.bcd2_to_byte(buf[2] & 0x3f)
        return i2c_bm8563_time_struct

    def set_time(self, i2c_bm8563_time_struct):
        if i2c_bm8563_time_struct is None:
            return
        self._i2cPort.writeto(self._deviceAddress, bytearray([
            0x02,
            self.byte_to_bcd2(i2c_bm8563_time_struct.seconds),
            self.byte_to_bcd2(i2c_bm8563_time_struct.minutes),
            self.byte_to_bcd2(i2c_bm8563_time_struct.hours)
        ]))

    def get_date(self):
        i2c_bm8563_date_struct:I2C_BM8563_DateTypeDef = I2C_BM8563_DateTypeDef(0,0,0,0)
        buf = bytearray(4)
        self._i2cPort.writeto(self._deviceAddress, bytearray([0x05]))
        self._i2cPort.readfrom_into(self._deviceAddress, buf)

        i2c_bm8563_date_struct.date = self.bcd2_to_byte(buf[0] & 0x3f)
        i2c_bm8563_date_struct.weekDay = self.bcd2_to_byte(buf[1] & 0x07)
        i2c_bm8563_date_struct.month = self.bcd2_to_byte(buf[2] & 0x1f)
        if buf[2] & 0x80:
            i2c_bm8563_date_struct.year = 1900 + self.bcd2_to_byte(buf[3] & 0xff)
        else:
            i2c_bm8563_date_struct.year = 2000 + self.bcd2_to_byte(buf[3] & 0xff)
        return i2c_bm8563_date_struct

    def setDate(self, year, month, date, weekDay):
        if year < 2000:
            month_data = self.byte_to_bcd2(month) | 0x80
            year_data = self.byte_to_bcd2(year % 100)
        else:
            month_data = self.byte_to_bcd2(month)
            year_data = self.byte_to_bcd2(year % 100)

        self._i2cPort.writeto(self._deviceAddress, bytes([0x05,
                                                          self.byte_to_bcd2(date),
                                                          self.byte_to_bcd2(weekDay),
                                                          month_data,
                                                          year_data]))

    def WriteReg(self, reg, value):
        self._i2cPort.writeto(self._deviceAddress, bytes([reg, value]))

    def ReadReg(self, reg):
        self._i2cPort.writeto(self._deviceAddress, bytes([reg]))
        buf = bytearray(1)
        self._i2cPort.readfrom_into(self._deviceAddress, buf)
        return buf[0]

    def SetAlarmIRQ(self, hour, minute):
        self.WriteReg(0x09, self.byte_to_bcd2(minute) | 0x80)
        self.WriteReg(0x0A, self.byte_to_bcd2(hour) | 0x80)

    def ClearAlarmIRQ(self):
        data = self.ReadReg(0x01)
        self.WriteReg(0x01, data & 0xfe)

    def clearIRQ(self):
        data = self.ReadReg(0x01)
        self.WriteReg(0x01, data & 0xf3)

    def disableIRQ(self):
        self.clearIRQ()
        data = self.ReadReg(0x01)
        self.WriteReg(0x01, data & 0xfC)
