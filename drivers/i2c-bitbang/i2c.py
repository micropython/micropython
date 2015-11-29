import sys
import utime
from machine import Pin


class I2C:

    def __init__(self, sda, scl):
        self.sda = sda
        self.scl = scl
        self.sda.init(Pin.IN)
        self.scl.init(Pin.IN)
        self.sda(0)
        self.scl(0)

    def sda_high(self):
        self.sda.init(Pin.IN)

    def sda_low(self):
        self.sda.init(Pin.OUT)

    def scl_high(self):
        self.scl.init(Pin.IN)

    def scl_low(self):
        self.scl.init(Pin.OUT)

    def delay(self):
        utime.sleep_us(5)

    def start(self):
        self.sda_low()
        self.delay()
        self.scl_low()
        self.delay()

    def restart(self):
        self.sda_high();
        self.delay();
        self.scl_high();
        self.delay();
        self.start();

    def stop(self):
        self.sda_low();
        self.scl_high();
        self.delay();
        self.sda_high();
        self.delay();

    def readbit(self):
        self.sda_high();
        self.scl_high();
        self.delay();
        bit = self.sda();
        self.scl_low();
        return bit

    def writebit(self, val):
        if val:
            self.sda_high();
        else:
            self.sda_low();
        self.delay();
        self.scl_high();
        self.delay(); # Rather should sense scl for clock stretching
        self.scl_low();

    def writebyte(self, b):
        for i in range(8):
            self.writebit(b & 0x80);
            b <<= 1;
        return self.readbit();

    def readbyte(self, ack=True):
        b = 0;
        for i in range(8):
            b <<= 1;
            b |= self.readbit()
        self.writebit(not ack);
        return b

    def mem_write(self, data, i2c_addr, mem_addr):
        self.start()
        print(i2c.writebyte(i2c_addr << 1))
        print(i2c.writebyte(mem_addr))
        for b in data:
            print(i2c.writebyte(b))
        self.stop()

    def mem_read(self, sz, i2c_addr, mem_addr):
        self.start()
        print(i2c.writebyte(i2c_addr << 1))
        print(i2c.writebyte(mem_addr))
        self.restart()
        print(i2c.writebyte((i2c_addr << 1) | 1))
        res = b""
        while sz:
            sz -= 1
            ack = bool(sz)
            b = self.readbyte(ack)
            res += bytes([b])
        self.stop()
        return res

# Accelerometer needs to be powered on first. Even
# though signal is called "AVDD", and there's separate
# "DVDD", without AVDD, it won't event talk on I2C bus.
accel_pwr = Pin("MMA_AVDD")
accel_pwr.value(1)

sda = Pin("X10", Pin.IN)
scl = Pin("X9", Pin.IN)
i2c = I2C(sda, scl)

i2c.start()
print("presence:", i2c.writebyte(0x4c << 1))
i2c.stop()

ACCEL_ADDR = 0x4c
ACCEL_AXIS_X_REG = 0
ACCEL_MODE_REG = 7

# Now activate measurements
i2c.mem_write(b"\x01", ACCEL_ADDR, ACCEL_MODE_REG)

while 1:
    val = i2c.mem_read(1, ACCEL_ADDR, ACCEL_AXIS_X_REG)
    print(val)
    utime.sleep(1)
