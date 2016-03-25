"""
LSM9DS1 - 9DOF inertial sensor of STMicro driver for MicroPython

The sensor contains an accelerometer / gyroscope / magnetometer
The magnetometer is using a different I2C address!

Uses the internal FIFO to store up to 16 gyro/accel data, use
the iter_accel_gyro generator to access it.

Example usage:
>>> from lsm9ds1 import LSM9DS1
>>> lsm = LSM9DS1(I2C(1, I2C.MASTER))
>>> lsm.read_gyro()       # (x,y,z) in deg/sec
(-0.4037476, 0.8224488, -0.05233765)
>>> lsm.read_accel()      # (x,y,z) in g
(-0.009338379, 0.07415771, 0.9942017)
>>> lsm.read_magnet()     # (x,y,z) in gauss 
(0.5358887, -0.001586914, -0.1228027)
>>> for g,a in lsm.iter_accel_gyro(): print(g,a)    # using fifo
"""
import array

class LSM9DS1:
    WHO_AM_I = const(0xf)
    CTRL_REG1_G = const(0x10)
    INT_GEN_SRC_G = const(0x14)
    OUT_TEMP = const(0x15)  
    OUT_G = const(0x18) 
    CTRL_REG4_G = const(0x1e)
    
    STATUS_REG = const(0x27)
    OUT_XL = const(0x28)
    FIFO_CTRL_REG = const(0x2e)
    FIFO_SRC = const(0x2f)
    
    OFFSET_REG_X_M = const(0x05)
    CTRL_REG1_M = const(0x20)
    OUT_M = const(0x28)
    
    SCALE_GYRO = [(245,0),(500,1),(2000,3)]
    SCALE_ACCEL = [(2,0),(4,2),(8,3),(16,1)]
    
    def __init__(self, i2c, address_gyro=106, address_magnet=28):
        self.i2c = i2c
        self.address_gyro = address_gyro
        self.address_magnet = address_magnet
        # check id's of accelerometer/gyro and magnetometer
        if (self.read_id_magnet() != b'=') or (self.read_id_gyro() != b'h'):
            raise OSError("Invalid LSM9DS1 device, using address {}/{}".format(
                    address_gyro,address_magnet))
        # allocate scratch buffer for efficient conversions and memread op's
        self.scratch = array.array('B',[0,0,0,0,0,0])
        self.scratch_int = array.array('h',[0,0,0])
        self.init_gyro_accel()
        self.init_magnetometer()
        
    def init_gyro_accel(self, sample_rate=6, scale_gyro=0, scale_accel=0):
        """ Initalizes Gyro and Accelerator.
        sample rate: 0-6 (off, 14.9Hz, 59.5Hz, 119Hz, 238Hz, 476Hz, 952Hz)
        scale_gyro: 0-2 (245dps, 500dps, 2000dps ) 
        scale_accel: 0-3 (+/-2g, +/-4g, +/-8g, +-16g)
        """
        assert sample_rate <= 6, "invalid sampling rate: %d" % sample_rate
        assert scale_gyro <= 2, "invalid gyro scaling: %d" % scale_gyro
        assert scale_accel <= 3, "invalid accelerometer scaling: %d" % scale_accel
        
        i2c = self.i2c
        addr = self.address_gyro
        mv = memoryview(self.scratch)        
        # angular control registers 1-3 / Orientation
        mv[0] = ((sample_rate & 0x07) << 5) | ((self.SCALE_GYRO[scale_gyro][1] & 0x3) << 3) 
        mv[1:4] = b'\x00\x00\x00'
        i2c.mem_write(mv[:5], addr, CTRL_REG1_G)
        # ctrl4 - enable x,y,z, outputs, no irq latching, no 4D
        # ctrl5 - enable all axes, no decimation
        # ctrl6 - set scaling and sample rate of accel 
        # ctrl7,8 - leave at default values
        # ctrl9 - FIFO enabled
        mv[0] = mv[1] = 0x38
        mv[2] = ((sample_rate & 7) << 5) | ((self.SCALE_ACCEL[scale_accel][1] & 0x3) << 3)
        mv[3] = 0x00
        mv[4] = 0x4
        mv[5] = 0x2
        i2c.mem_write(mv[:6], addr, CTRL_REG4_G )
        
        # fifo: use continous mode (overwrite old data if overflow)
        i2c.mem_write(0x00, addr, FIFO_CTRL_REG)
        i2c.mem_write(6 << 5, addr, FIFO_CTRL_REG)
        
        self.scale_gyro = 32768 / self.SCALE_GYRO[scale_gyro][0]
        self.scale_accel = 32768 / self.SCALE_ACCEL[scale_accel][0]
        
    def init_magnetometer(self, sample_rate=7, scale_magnet=0):
        """ 
        sample rates = 0-7 (0.625, 1.25, 2.5, 5, 10, 20, 40, 80Hz)
        scaling = 0-3 (+/-4, +/-8, +/-12, +/-16 Gauss)
        """
        assert sample_rate < 8, "invalid sample rate: %d (0-7)" % sample_rate
        assert scale_magnet < 4, "invalid scaling: %d (0-3)" % scale_magnet
        i2c = self.i2c
        addr = self.address_magnet
        mv = memoryview(self.scratch)       
        mv[0] = 0x40 | (sample_rate << 2) # ctrl1: high performance mode
        mv[1] = scale_magnet << 5 # ctrl2: scale, normal mode, no reset
        mv[2] = 0x00 # ctrl3: continous conversion, no low power, I2C
        mv[3] = 0x08 # ctrl4: high performance z-axis
        mv[4] = 0x00 # ctr5: no fast read, no block update
        i2c.mem_write(mv[:5], addr, CTRL_REG1_M)
        self.scale_factor_magnet = 32768 / ((scale_magnet+1) * 4 )
        
    def calibrate_magnet(self, offset):
        """ 
        offset is a magnet vecor that will be substracted by the magnetometer
        for each measurement. It is written to the magnetometer's offset register
        """
        offset = [int(i*self.scale_factor_magnet) for i in offset]
        mv = memoryview(self.scratch) 
        mv[0] = offset[0] & 0xff
        mv[1] = offset[0] >> 8
        mv[2] = offset[1] & 0xff
        mv[3] = offset[1] >> 8
        mv[4] = offset[2] & 0xff
        mv[5] = offset[2] >> 8
        self.i2c.mem_write(mv[:6], self.address_magnet, OFFSET_REG_X_M)
                
    def read_id_gyro(self):
        return self.i2c.mem_read(1, self.address_gyro, WHO_AM_I)
    
    def read_id_magnet(self):
        return self.i2c.mem_read(1, self.address_magnet, WHO_AM_I)
                        
    def read_magnet(self):
        """Returns magnetometer vector in gauss.
        raw_values: if True, the non-scaled adc values are returned
        """
        mv = memoryview(self.scratch_int)
        f = self.scale_factor_magnet
        self.i2c.mem_read(mv, self.address_magnet, OUT_M | 0x80)
        return (mv[0]/f, mv[1]/f, mv[2]/f)
    
    def read_gyro(self):
        """Returns gyroscope vector in degrees/sec."""
        mv = memoryview(self.scratch_int)
        f = self.scale_gyro
        self.i2c.mem_read(mv, self.address_gyro, OUT_G | 0x80)
        return (mv[0]/f, mv[1]/f, mv[2]/f)
    
    def read_accel(self):
        """Returns acceleration vector in gravity units (9.81m/s^2)."""
        mv = memoryview(self.scratch_int)
        f = self.scale_accel
        self.i2c.mem_read(mv, self.address_gyro, OUT_XL | 0x80)
        return (mv[0]/f, mv[1]/f, mv[2]/f)
        
    def iter_accel_gyro(self):
        """A generator that returns tuples of (gyro,accelerometer) data from the fifo."""
        while True:
            fifo_state = self.i2c.mem_read(1, self.address_gyro, FIFO_SRC)[0]
            if fifo_state & 0x3f:
                # print("Available samples=%d" % (fifo_state & 0x1f))
                yield self.read_gyro(),self.read_accel()
            else:
                break
    
