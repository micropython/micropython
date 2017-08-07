"""
HTS221 - STMicro I2C temperature and humidity sensor driver for MicroPython
Example usage:
>>> from hts221 import HTS221
>>> hts = HTS221(I2C(1))
>>> hts.measure()
>>> hts.get_humidity()
62.07
>>> hts.get_temperature()
27.046
"""
import uctypes
import time

class HTS221:    
    HTS_WHO_AM_I = const(0xf) 
    def __init__(self, i2c, addr=95, fixed=False):
        self.i2c = i2c
        self.addr = addr
        self.fixed = fixed
        if (self.i2c.readfrom_mem(addr, HTS_WHO_AM_I, 1) != b'\xbc'):
            raise OSError("No HTS221 device on address {} found".format(addr))
        self.cal_buf = bytearray(16)
        self.calib = uctypes.struct(uctypes.addressof(self.cal_buf), {
                'h0_rh':(uctypes.UINT8 | 0), 
                'h1_rh':(uctypes.UINT8 | 1), 
                't0_deg':(uctypes.UINT8 | 2), 
                't1_deg':(uctypes.UINT8 | 3), 
                'res':(uctypes.UINT8 | 4),
                't1t0msb':(uctypes.UINT8 | 5),
                'h0_out':(uctypes.INT16 | 6),
                'res1':(uctypes.INT16 | 8),
                'h1_out':(uctypes.INT16 | 10),
                't0_out':(uctypes.INT16 | 12),
                't1_out':(uctypes.INT16 | 14)})
        self.val_buf = bytearray(5)
        self.raw_val = uctypes.struct(uctypes.addressof(self.val_buf), {
                'status':(uctypes.UINT8 | 0), 
                'h_out':(uctypes.INT16 | 1), 
                't_out':(uctypes.INT16 | 3)})        
        # enable sensor, one-shot
        i2c.writeto_mem(addr, 0x20, b'\x84')
        i2c.readfrom_mem_into(95, 0x30|0x80, self.cal_buf)
        # add 2 bits from reg 0x35 -> 10bit unsigned values)
        self.t0_deg = (self.calib.t0_deg | ((self.calib.t1t0msb & 0x3) << 8))
        self.t1_deg = (self.calib.t1_deg | ((self.calib.t1t0msb & 0xc) << 6))
        # multiply with large base to keep it accurate for fixed point division
        self.t_slope = (1000 * (self.t1_deg - self.t0_deg)) // (self.calib.t1_out - self.calib.t0_out)
        self.h_slope = (10000 * (self.calib.h1_rh - self.calib.h0_rh)) // (self.calib.h1_out - self.calib.h0_out)
        
    def measure(self):
        # enable one-shot measurement
        self.i2c.writeto_mem(self.addr, 0x21, b'\x01')
        self.measure_done = False

    def measure_end(self):
        # blocking, timeout after 0.2sec
        delay_cntr = 0
        while (delay_cntr < 20):
            self.i2c.readfrom_mem_into(self.addr, 0x27|0x80, self.val_buf)
            if (self.raw_val.status & 0x3):
                break
            time.sleep_ms(10)
            delay_cntr += 1
        else:
            raise OSError("Timeout Sensor")
        self.temperature = (1000 * self.t0_deg + self.t_slope * (self.raw_val.t_out - self.calib.t0_out)) // 8
        self.humidity = (10000 * self.calib.h0_rh + self.h_slope * (self.raw_val.h_out - self.calib.h0_out)) // 200
        if self.fixed:
            self.temperature = (self.temperature << 8 ) | (-3 + 128)
            self.humidity = (self.humidity << 8) | (-2 + 128)
        else:
            self.temperature = self.temperature / 1000.
            self.humidity = self.humidity / 100.
        self.measure_done = True

    def set_heater(self, switch_on):
        if switch_on: 
            self.i2c.writeto_mem(self.addr, 0x21, b'\x02')
        else:
            self.i2c.writeto_mem(self.addr, 0x21, b'\x00')

    def fixed_to_float(self,fx):
        # may be moved to a generic location
        return (fx >> 8) * 10**((fx & 0xff) - 128)

    def get_humidity(self):
        if not self.measure_done:
            self.measure_end()
        return self.humidity

    def get_temperature(self):
        if not self.measure_done:
            self.measure_end()
        return self.temperature