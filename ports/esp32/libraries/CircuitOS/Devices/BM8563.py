from machine import I2C


def bcd2dec(bcd: int) -> int:
    return ((bcd >> 4) * 10) + (bcd & 0x0f)


def dec2bcd(dec: int) -> int:
    return ((dec // 10) << 4) | (dec % 10)


class BM8563:
    ADDR = const(0x51)
    time = (0, 0, 0, 0, 0, 0, 0)

    def __init__(self, i2c: I2C, address: int = ADDR):
        self._i2c = i2c
        self._addr = address

    def begin(self) -> bool:
        try:
            self._i2c.writeto(self._addr, bytearray([0x0, 0x0, 0x0]))  # clear status registers
            return True
        except OSError:
            return False

    def get_time(self) -> tuple[int, int, int, int, int, int, int]:
        self._i2c.writeto(self._addr, bytearray([0x02]))

        data = self._i2c.readfrom(self._addr, 7)

        # 0..59
        sec = bcd2dec(data[0] & 0b01111111)

        # 0..59
        mins = bcd2dec(data[1] & 0b01111111)

        # 0..23
        hour = bcd2dec(data[2] & 0b00111111)

        # 1..31
        day = bcd2dec(data[3] & 0b00111111)

        # 0..6
        weekday = bcd2dec(data[4] & 0b00000111)

        # 0..11
        month = bcd2dec(data[5] & 0b00011111) - 1

        '''If century bit set assume it is 2000. Note that it seems 
        that unlike PCF8563, the BM8563 does NOT automatically 
        toggle the century bit when year overflows from 99 to 00. '''

        century = 100 if (data[5] & 0b10000000) else 0

        # Number of years since 1900
        year = bcd2dec(data[6] & 0b11111111) + century + 1900

        time = (year, month, day, weekday, hour, mins, sec)

        return time

    def set_time(self, time: tuple[int, int, int, int, int, int, int]):
        self.time = time
        data = bytearray(7)
        # (year, month, day, weekday, hour, mins, sec)

        # 0..59
        data[0] = dec2bcd(time[6]) & 0b01111111

        # 0..59
        data[1] = dec2bcd(time[5]) & 0b01111111

        # 0..23
        data[2] = dec2bcd(time[4]) & 0b00111111

        # 1..31
        data[3] = dec2bcd(time[2]) & 0b00111111

        # 0..6
        data[4] = dec2bcd(time[3]) & 0b00000111

        # 1..12
        data[5] = dec2bcd(time[1] + 1) & 0b00011111

        # If 2000 set the century bit.
        if time[0] >= 100:
            data[5] |= 0b10000000

        # 0..99
        data[6] = dec2bcd(time[0] % 100) & 0b11111111

        wdata = bytearray(8)
        wdata[0] = 0x02
        for i in range(7):
            wdata[i + 1] = data[i]

        self._i2c.writeto(self._addr, wdata)

    def get_seconds(self) -> int:
        return self.get_time()[6]

    def set_seconds(self, seconds: int):
        self.set_time((self.time[0], self.time[1], self.time[2], self.time[3], self.time[4], self.time[5], seconds))

    def get_minutes(self) -> int:
        return self.get_time()[5]

    def set_minutes(self, minutes: int):
        self.set_time((self.time[0], self.time[1], self.time[2], self.time[3], self.time[4], minutes, self.time[6]))

    def get_hours(self) -> int:
        return self.get_time()[4]

    def set_hours(self, hours: int):
        self.set_time((self.time[0], self.time[1], self.time[2], self.time[3], hours, self.time[5], self.time[6]))

    def get_year(self) -> int:
        return self.get_time()[0]

    def set_year(self, year: int):
        self.set_time((year, self.time[1], self.time[2], self.time[3], self.time[4], self.time[5], self.time[6]))

    def get_month(self) -> int:
        return self.get_time()[1]

    def set_month(self, month: int):
        self.set_time((self.time[0], month, self.time[2], self.time[3], self.time[4], self.time[5], self.time[6]))

    def get_day(self) -> int:
        return self.get_time()[2]

    def set_day(self, day: int):
        self.set_time((self.time[0], self.time[1], day, self.time[3], self.time[4], self.time[5], self.time[6]))

    def get_weekday(self) -> int:
        return self.get_time()[3]

    def set_weekday(self, weekday: int):
        self.set_time((self.time[0], self.time[1], self.time[2], weekday, self.time[4], self.time[5], self.time[6]))
