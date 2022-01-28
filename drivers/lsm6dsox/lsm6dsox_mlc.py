# LSM6DSOX IMU MLC (Machine Learning Core) Example.
# Download the raw UCF file, copy to storage and reset.

# NOTE: The pre-trained models (UCF files) for the examples can be found here:
# https://github.com/STMicroelectronics/STMems_Machine_Learning_Core/tree/master/application_examples/lsm6dsox

import time
from lsm6dsox import LSM6DSOX
from machine import Pin, I2C

INT_MODE = True  # Run in interrupt mode.
INT_FLAG = False  # Set True on interrupt.


def imu_int_handler(pin):
    global INT_FLAG
    INT_FLAG = True


if INT_MODE == True:
    int_pin = Pin(24)
    int_pin.irq(handler=imu_int_handler, trigger=Pin.IRQ_RISING)

i2c = I2C(0, scl=Pin(13), sda=Pin(12))

# Vibration detection example
UCF_FILE = "lsm6dsox_vibration_monitoring.ucf"
UCF_LABELS = {0: "no vibration", 1: "low vibration", 2: "high vibration"}
# NOTE: Selected data rate and scale must match the MLC data rate and scale.
lsm = LSM6DSOX(i2c, gyro_odr=26, accel_odr=26, gyro_scale=2000, accel_scale=4, ucf=UCF_FILE)

# Head gestures example
# UCF_FILE = "lsm6dsox_head_gestures.ucf"
# UCF_LABELS = {0:"Nod", 1:"Shake", 2:"Stationary", 3:"Swing", 4:"Walk"}
# NOTE: Selected data rate and scale must match the MLC data rate and scale.
# lsm = LSM6DSOX(i2c, gyro_odr=26, accel_odr=26, gyro_scale=250, accel_scale=2, ucf=UCF_FILE)

print("MLC configured...")

while True:
    if INT_MODE:
        if INT_FLAG:
            INT_FLAG = False
            print(UCF_LABELS[lsm.read_mlc_output()[0]])
    else:
        buf = lsm.read_mlc_output()
        if buf != None:
            print(UCF_LABELS[buf[0]])
