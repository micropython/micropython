# Dev by Sonthaya Nongnuch

from machine import Pin, I2C

MCP79411_ADDR = const(0x6F)

def datetime(value=()):
    i2c1 = I2C(1, scl=Pin(5), sda=Pin(4), freq=100000)
    
    def bcd2dec(x):
        return int(((x >> 4) * 10) + (x & 0x0F))
        
    def dec2bcd(x):
        return (int(x / 10) << 4) | (x % 10)

    if len(value) == 0: # Read
        data = i2c1.readfrom_mem(MCP79411_ADDR, 0x00, 7)
        year = bcd2dec(data[6]) + 2000
        month = bcd2dec(data[5] & 0x1F)
        day = bcd2dec(data[4] & 0x3F)
        hour = bcd2dec(data[2] & 0x3F)
        minute = bcd2dec(data[1] & 0x7F)
        second = bcd2dec(data[0] & 0x7F)
        microsecond = 0
        tzinfo = 0
        return (year, month, day, hour, minute, second, microsecond, tzinfo)
    else: # Write
        data = bytearray(7)
        
        year = value[0] if len(value) >= 1 else 0
        month = value[1] if len(value) >= 2 else 1
        day = value[2] if len(value) >= 3 else 1
        hour = value[3] if len(value) >= 4 else 0
        minute = value[4] if len(value) >= 5 else 0
        second = value[5] if len(value) >= 6 else 0
        microsecond = value[6] if len(value) >= 7 else 0
        tzinfo = value[7] if len(value) >= 8 else 0

        data[0] = (dec2bcd(second) & 0x7F) | 0x80
        data[1] = dec2bcd(minute) & 0x7F
        data[2] = dec2bcd(hour) & 0x3F
        data[3] = 0x01
        data[4] = dec2bcd(day) & 0x3F
        data[5] = dec2bcd(month) & 0x1F
        data[6] = dec2bcd(year - 2000) & 0xFF

        i2c1.writeto_mem(MCP79411_ADDR, 0x00, data)
        return True
