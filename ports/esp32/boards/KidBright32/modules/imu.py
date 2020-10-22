# Dev by Sonthaya Nongnuch

from machine import Pin, I2C
import math
import utime

MPU6050_ADDR = const(0x68)
LSM303AGR_ACC_ADDR = const(0x19)
LSM303AGR_MEG_ADDR = const(0x1E)

MPU6050 = const(0)
LSM303AGR = const(1)

acc = [0, 0, 0, 0]
gyro = [0, 0, 0, 0]
mag = [0, 0, 0, 0]
temp = 0.0

EVENT_SHAKE = const(0)
EVENT_BOARD_UP = const(1)
EVENT_BOARD_DOWN = const(2)
EVENT_SCREEN_UP = const(3)
EVENT_SCREEN_DOWN = const(4)
EVENT_TILT_LEFT = const(5)
EVENT_TILT_RIGHT = const(6)
EVENT_FREE_FALL = const(7)
EVENT_3G = const(8)
EVENT_6G = const(9)
EVENT_8G = const(10)

__event_callback = [ None ] * 10

__startCalcLowStrengthContinue = False
__xStartCalc = 0

# Check sensor on board
i2c1 = I2C(1, scl=Pin(5), sda=Pin(4), freq=100000)
devAddrs = i2c1.scan()
__device = -1
if MPU6050_ADDR in devAddrs:
    __device = MPU6050
elif LSM303AGR_ACC_ADDR in devAddrs:
    __device = LSM303AGR
del devAddrs

if __device == MPU6050:
    i2c1.writeto_mem(MPU6050_ADDR, 0x6B, b'\x00')
    i2c1.writeto_mem(MPU6050_ADDR, 0x1C, b'\x10')  # 8g
    i2c1.writeto_mem(MPU6050_ADDR, 0x1B, b'\x00')  # 250 */sec
elif __device == LSM303AGR:
    i2c1.writeto_mem(LSM303AGR_ACC_ADDR, 0x2E, b'\x00')  # FIFO_CTRL_REG_A
    i2c1.writeto_mem(LSM303AGR_ACC_ADDR, 0x20, b'\x57')  # CTRL_REG1_A
    i2c1.writeto_mem(LSM303AGR_ACC_ADDR, 0x23, b'\x20')  # Full-scale selection, 8g

    i2c1.writeto_mem(LSM303AGR_MEG_ADDR, 0x60, b'\x8C')  # CFG_REG_A_M
    i2c1.writeto_mem(LSM303AGR_MEG_ADDR, 0x61, b'\x01')  # CFG_REG_B_M


def b2i(x, y):
    return (x << 8 | y) if not x & 0x80 else (-(((x ^ 255) << 8) | (y ^ 255) + 1))

def i2b(n):
    if n < 0:
        n = (-n ^ 0xFFFF) + 1
    return bytes([ (n >> 8) & 0xFF, n & 0xFF ])

def update():
    global acc, gyro, mag, temp
    if __device == MPU6050:
        buff = i2c1.readfrom_mem(MPU6050_ADDR, 0x3B, 14)
        for i in range(3):
            acc[i] = b2i(buff[(i * 2)], buff[(i * 2) + 1])
            acc[i] = round(acc[i] / 4096.0 * 1000.0, 2)
        x = -acc[1]
        y = acc[0]
        z = -acc[2]
        acc[0] = x
        acc[1] = y
        acc[2] = z
        acc[3] = math.sqrt(math.pow(acc[0], 2) + math.pow(acc[1], 2) + math.pow(acc[2], 2))
        del x, y, z
        temp = round(b2i(buff[6], buff[7]) / 340.00 + 36.53, 2)
        for i in range(3):
            gyro[i] = b2i(buff[(i * 2) + 8], buff[(i * 2) + 9])
            gyro[i] = round(gyro[i] / 131.0, 2)
        gyro[3] = math.sqrt(math.pow(gyro[0], 2) + math.pow(gyro[1], 2) + math.pow(gyro[2], 2))
    elif __device == LSM303AGR:
        buff = [0, 0, 0, 0, 0, 0]
        for i in range(6):
            value = i2c1.readfrom_mem(LSM303AGR_ACC_ADDR, 0x28 + i, 1)[0]
            buff[i] = value
        for i in range(3):
            acc[i] = b2i(buff[(i * 2) + 1], buff[(i * 2) + 0])
            acc[i] = round(((acc[i] >> 6) * 15630 + 500) / 1000, 2)
        acc[3] = math.sqrt(math.pow(acc[0], 2) + math.pow(acc[1], 2) + math.pow(acc[2], 2))
        acc[0] = acc[0] * -1
        acc[1] = acc[1] * -1
        buff = [0, 0, 0, 0, 0, 0]
        for i in range(6):
            buff[i] = i2c1.readfrom_mem(LSM303AGR_MEG_ADDR, 0x68 + i, 1)[0]
        for i in range(3):
            mag[i] = b2i(buff[(i * 2) + 1], buff[(i * 2) + 0])
            mag[i] = round(mag[i] * 1.5 * 0.1, 2)
        mag[3] = math.sqrt(math.pow(mag[0], 2) + math.pow(mag[1], 2) + math.pow(mag[2], 2))

def rotation():
    x_g_value = acc[0] / 1000.0 # Acceleration in x-direction in g units
    y_g_value = acc[1] / 1000.0 # Acceleration in y-direction in g units
    z_g_value = -acc[2] / 1000.0 # Acceleration in z-direction in g units

    roll = (((math.atan2(z_g_value, x_g_value) * 180) / 3.14) + 180)
    if roll >= 270:
        roll = 270 - roll
    elif roll >= 90:
        roll = math.fmod(90 - roll, -180) + 180
    else:
        roll = -90 - roll
    roll = round(roll, 2)

    pitch = (((math.atan2(y_g_value, z_g_value) * 180) / 3.14) + 180)
    pitch = 180 - pitch
    pitch = round(pitch, 2)

    return (roll, pitch)

calibrateMagFlag = False
mag_min = [ 99999, 99999, 99999 ]
mag_max = [ -99999, -99999, -99999 ]
def heading():
    global calibrateMagFlag
    if not calibrateMagFlag:
        if not loadCalibrateFromSRAM():
            calibrate_compass()
        calibrateMagFlag = True

    # use calibration values to shift and scale magnetometer measurements
    x_mag = (0.0 + mag[0] - mag_min[0]) / (mag_max[0] - mag_min[0]) * 2 - 1
    y_mag = (0.0 + mag[1] - mag_min[1]) / (mag_max[1] - mag_min[1]) * 2 - 1
    z_mag = (0.0 + mag[2] - mag_min[2]) / (mag_max[2] - mag_min[2]) * 2 - 1

    # Normalize acceleration measurements so they range from 0 to 1
    s = math.sqrt(math.pow(acc[0], 2) + math.pow(acc[1], 2) + math.pow(acc[2], 2))
    xAccelNorm = acc[0] / s
    yAccelNorm = acc[1] / s
    # DF("Acc norm (x, y): (%f, %f)\n", xAccelNorm, yAccelNorm)

    pitch = math.asin(-xAccelNorm)
    roll = math.asin(yAccelNorm / math.cos(pitch))

    # tilt compensated magnetic sensor measurements
    x_mag_comp = x_mag * math.cos(pitch) + z_mag * math.sin(pitch)
    y_mag_comp = x_mag * math.sin(roll) * math.sin(pitch) + y_mag * math.cos(roll) - z_mag * math.sin(roll) * math.cos(pitch)

    # arctangent of y/x converted to degrees
    heading = 180 * math.atan2(x_mag_comp, y_mag_comp) / math.pi

    heading = (-heading) if heading <= 0 else (360 - heading)
    return int(heading)


def calibrate_compass():
    global calibrateMagFlag, mag_min, mag_max
    import display
    from time import sleep

    mag_min = [ 99999, 99999, 99999 ]
    mag_max = [ -99999, -99999, -99999 ]

    display.scroll("TILT TO FILL SCREEN")

    buffer = bytearray(16)

    before_index_cols = -1
    before_index_rows = -1

    delay = 0
    pixelShow = False
    while True:
        if mag[0] < mag_min[0]:
            mag_min[0] = mag[0]
        if mag[0] > mag_max[0]:
            mag_max[0] = mag[0]

        if mag[1] < mag_min[1]:
            mag_min[1] = mag[1]
        if mag[1] > mag_max[1]:
            mag_max[1] = mag[1]
        
        if mag[2] < mag_min[2]:
            mag_min[2] = mag[2]
        if mag[2] > mag_max[2]:
            mag_max[2] = mag[2]

        update()
        (roll, pitch) = rotation()

        index_cols = int((roll - -60) * (15 - 0) / (60 - -60) + 0)
        index_rows = int((pitch - -60) * (7 - 0) / (60 - -60) + 0)

        if index_cols > 15:
            index_cols = 15
        if index_rows > 7:
            index_rows = 7

        if before_index_cols == -1 or before_index_rows == -1:
            before_index_cols = index_cols
            before_index_rows = index_rows

        if index_cols >= 0 and index_cols <= 15 and index_rows >= 0 and index_rows <= 7:
            if (index_cols != before_index_cols) or (index_rows != before_index_rows):
                buffer[before_index_cols] |= 0x80 >> before_index_rows

            if pixelShow:
                buffer[index_cols] |= 0x80 >> index_rows
                if buffer == b'\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF':
                    break
            else:
                buffer[index_cols] &= ~(0x80 >> index_rows)
            if delay >= 200:
                delay = 0
                pixelShow = not pixelShow

        before_index_cols = index_cols
        before_index_rows = index_rows

        display.raw(buffer)

        sleep(0.05)
        delay += 50

    display.scroll("FINISH")
    saveCalibrateIntoSRAM()
    calibrateMagFlag = True

def loadCalibrateFromSRAM():
    buff = i2c1.readfrom_mem(0x6F, 0x20 + 0, 13)
    crc = 0
    for i in range(12):
        crc = crc + buff[i]
    crc = crc & 0xFF

    if crc != buff[12]:
        return False

    for i in range(3):
        mag_min[i] = b2i(buff[(i * 2) + 0], buff[(i * 2) + 1]) / 100.0
    for i in range(3):
        mag_max[i] = b2i(buff[(i * 2) + 0 + 6], buff[(i * 2) + 1 + 6]) / 100.0

    return True

def saveCalibrateIntoSRAM():
    buff = bytearray(13)
    for i in range(3):
        a = i2b(int(mag_min[i] * 100))
        buff[(i * 2) + 0] = a[0]
        buff[(i * 2) + 1] = a[1]
    for i in range(3):
        a = i2b(int(mag_max[i] * 100))
        buff[(i * 2) + 0 + 6] = a[0]
        buff[(i * 2) + 1 + 6] = a[1]
    crc = 0
    for i in range(12):
        crc = crc + buff[i]
    buff[12] = crc & 0xFF

    i2c1.writeto_mem(0x6F, 0x20 + 0, buff)
    return True

def is_gesture(event, blocking=True):
    global __xStartCalc, __startCalcLowStrengthContinue
    if event == EVENT_SHAKE:
        return acc[3] > 4000
    elif event == EVENT_BOARD_UP:
        return acc[1] < -600
    elif event == EVENT_BOARD_DOWN:
        return acc[1] > 600
    elif event == EVENT_SCREEN_UP:
        pitch = rotation()[1]
        return pitch >= -30 and pitch <= 30
    elif event == EVENT_SCREEN_DOWN:
        pitch = rotation()[1]
        return pitch >= 150 or pitch <= -150
    elif event == EVENT_TILT_LEFT:
        roll = rotation()[0]
        return roll <= -30
    elif event == EVENT_TILT_RIGHT:
        roll = rotation()[0]
        return roll >= 30
    elif event == EVENT_FREE_FALL:
        if blocking:
            lowStrengthContinue = False
            for i in range(0, 240, 40):
                if acc[3] < 500:
                    lowStrengthContinue = True
                    utime.sleep_ms(40)
                else:
                    lowStrengthContinue = False
                    break
            return lowStrengthContinue
        else:
            if acc[3] < 500:
                if not __startCalcLowStrengthContinue:
                    __xStartCalc = utime.ticks_ms()
                    __startCalcLowStrengthContinue = True
                else:
                    if (utime.ticks_ms() - __xStartCalc) >= 220E3:
                        __startCalcLowStrengthContinue = False
                        return True
            else:
                __xStartCalc = 0
                __startCalcLowStrengthContinue = False
                return False
    elif event ==  EVENT_3G:
        return acc[3] > 3000

    elif event == EVENT_6G:
        return acc[3] > 6000

    elif event == EVENT_8G:
        return acc[3] > 8000
    else:
        return False
