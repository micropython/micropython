# Dev by Sonthaya Nongnuch

from machine import Pin, I2C

MPU6050_ADDR       = const(0x68)
LSM303AGR_ACC_ADDR = const(0x19)
LSM303AGR_MEG_ADDR = const(0x1E)

MPU6050   = const(0)
LSM303AGR = const(1)

acc  = [ 0, 0, 0 ]
gyro = [ 0, 0, 0 ]
mag  = [ 0, 0, 0 ]
temp = 0.0

# Check sensor on board
i2c1 = I2C(1, scl=Pin(5), sda=Pin(4), freq=100000)
devAddrs = i2c1.scan()
__device = -1
if MPU6050_ADDR in devAddrs:
    __device = MPU6050
elif LSM303AGR_ACC_ADDR in devAddrs:
    __device = LSM303AGR

if __device == MPU6050:
    i2c1.writeto_mem(MPU6050_ADDR, 0x6B, b'\x00')
    i2c1.writeto_mem(MPU6050_ADDR, 0x1C, b'\x00') # 2g
    i2c1.writeto_mem(MPU6050_ADDR, 0x1B, b'\x00') # 250 */sec
elif __device == LSM303AGR:
    i2c1.writeto_mem(LSM303AGR_ACC_ADDR, 0x2E, b'\x00') # FIFO_CTRL_REG_A
    i2c1.writeto_mem(LSM303AGR_ACC_ADDR, 0x20, b'\x57') # CTRL_REG1_A
    i2c1.writeto_mem(LSM303AGR_ACC_ADDR, 0x23, b'\x00') # Full-scale selection, 2g
    i2c1.writeto_mem(LSM303AGR_MEG_ADDR, 0x60, b'\x8C') # CFG_REG_A_M
    i2c1.writeto_mem(LSM303AGR_MEG_ADDR, 0x61, b'\x01') # CFG_REG_B_M


def b2i(x, y):
    return (x << 8 | y) if not x & 0x80 else (-(((x ^ 255) << 8) | (y ^ 255) + 1))

def update():
    global acc, gyro, mag, temp
    if __device == MPU6050:
        buff = i2c1.readfrom_mem(MPU6050_ADDR, 0x3B, 14)
        for i in range(3):
            acc[i] = round(b2i(buff[(i * 2)], buff[(i * 2) + 1]) / 16384.0 * 1000.0, 2)
        temp = round(b2i(buff[6], buff[7]) / 340.00 + 36.53, 2)
        for i in range(3):
            gyro[i] = round(b2i(buff[(i * 2) + 8], buff[(i * 2) + 9]) / 131.0, 2)
    elif __device == LSM303AGR:
        buff = [ 0, 0, 0, 0, 0, 0 ]
        for i in range(6):
            value = i2c1.readfrom_mem(LSM303AGR_ACC_ADDR, 0x28 + i, 1)[0]
            buff[i] = value
        for i in range(3):
            acc[i] = b2i(buff[(i * 2) + 1], buff[(i * 2) + 0])
            acc[i] = round(((acc[i] >> 6) * 3900 + 500) / 1000, 2)
        buff = [ 0, 0, 0, 0, 0, 0 ]
        for i in range(6):
            buff[i] = i2c1.readfrom_mem(LSM303AGR_MEG_ADDR, 0x68 + i, 1)[0]
        for i in range(3):
            mag[i] = b2i(buff[(i * 2) + 1], buff[(i * 2) + 0])
            mag[i] = round(mag[i] * 1.5, 2)
