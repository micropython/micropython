from machine import I2C
from micropython import const


def bcd2dec(bcd: int) -> int:
    return ((bcd >> 4) * 10) + (bcd & 0x0f)


def dec2bcd(dec: int) -> int:
    return ((dec // 10) << 4) | (dec % 10)

class Time:
    year = 0
    month = 0
    day = 0
    hours = 0
    minutes = 0
    seconds = 0

def clamp(val: int, floor: int, ceil: int):
    return max(floor, min(val, ceil))

class BM8563:
    ADDR = const(0x51)
    time = Time()

    def __init__(self, i2c: I2C, address: int = ADDR):
        self._i2c = i2c
        self._addr = address

    def begin(self) -> bool:
        try:
            self._i2c.writeto(self._addr, bytearray([0x0, 0x0, 0x0]))  # clear status registers
            self.get_time()
        except OSError:
            return False

        return True

    def get_time(self) -> Time:
        self._i2c.writeto(self._addr, bytearray([0x02]))

        data = self._i2c.readfrom(self._addr, 7)

        # 0..59
        self.time.seconds = bcd2dec(data[0] & 0b01111111)

        # 0..59
        self.time.minutes = bcd2dec(data[1] & 0b01111111)

        # 0..23
        self.time.hours = bcd2dec(data[2] & 0b00111111)

        # 1..31
        self.time.day = bcd2dec(data[3] & 0b00111111)

        # weekday ignored

        # 1..12
        self.time.month = bcd2dec(data[5] & 0b00011111)

        '''If century bit set assume it is 2000. Note that it seems 
        that unlike PCF8563, the BM8563 does NOT automatically 
        toggle the century bit when year overflows from 99 to 00. '''
        century = 100 if (data[5] & 0b10000000) else 0

        # Number of years since 1900
        yr1900 = bcd2dec(data[6] & 0b11111111)
        self.time.year = yr1900 + century + 1900

        return self.time

    def set_time(self, time: Time):
        data = bytearray(7)
        # (year, month, day, weekday, hour, mins, sec)

        # 0..59
        time.seconds = clamp(time.seconds, 0, 59)
        data[0] = dec2bcd(time.seconds) & 0b01111111

        # 0..59
        time.minutes = clamp(time.minutes, 0, 59)
        data[1] = dec2bcd(time.minutes) & 0b01111111

        # 0..23
        time.hours = clamp(time.hours, 0, 23)
        data[2] = dec2bcd(time.hours) & 0b00111111

        # 1..31
        time.day = clamp(time.day, 1, 31)
        data[3] = dec2bcd(time.day) & 0b00111111

        # 0..6 // weekday // ignored here
        # data[4] = dec2bcd(time[3]) & 0b00000111

        # 1..12
        time.month = clamp(time.month, 1, 12)
        data[5] = dec2bcd(time.month) & 0b00011111

        time.year = clamp(time.year, 1900, 2099)

        # If 2000 set the century bit.
        if time.year >= 2000:
            data[5] |= 0b10000000

        # 0..99
        data[6] = dec2bcd(time.year % 100) & 0b11111111

        wdata = bytearray(8)
        wdata[0] = 0x02
        for i in range(7):
            wdata[i + 1] = data[i]

        self._i2c.writeto(self._addr, wdata)
        self.get_time()

    def get_seconds(self) -> int:
        return self.get_time().seconds

    def set_seconds(self, seconds: int):
        time = self.get_time()
        time.seconds = seconds
        self.set_time(time)

    def get_minutes(self) -> int:
        return self.get_time().minutes

    def set_minutes(self, minutes: int):
        time = self.get_time()
        time.minutes = minutes
        self.set_time(time)

    def get_hours(self) -> int:
        return self.get_time().hours

    def set_hours(self, hours: int):
        time = self.get_time()
        time.hours = hours
        self.set_time(time)

    def get_day(self) -> int:
        return self.get_time().day

    def set_day(self, day: int):
        time = self.get_time()
        time.day = day
        self.set_time(time)

    def get_month(self) -> int:
        return self.get_time().month

    def set_month(self, month: int):
        time = self.get_time()
        time.month = month
        self.set_time(time)

    def get_year(self) -> int:
        return self.get_time().year

    def set_year(self, year: int):
        time = self.get_time()
        time.year = year
        self.set_time(time)
