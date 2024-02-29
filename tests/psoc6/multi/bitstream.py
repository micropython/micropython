import sys
import time
from machine import Pin
from machine import bitstream
import os

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    gpio_pin = "P13_6"
    pin_in = "P12_0"

elif "CY8CPROTO-063-BLE" in machine:
    gpio_pin = "P5_2"
    pin_in = "P5_3"

timing = [50000, 10000, 80000, 75000]
buf = bytearray([0xAB])
device = '/dev/ttyACM0'
file_location = "./psoc6/test_inputs/bitstream_listen.py"
remote_directory_path = "/"
mpr_connect = f"../tools/mpremote/mpremote.py connect {device}"
mpr_file_cp = f"{mpr_connect} cp {file_location} :{remote_directory_path}"
mpr_execute = f"../tools/mpremote/mpremote.py exec {remote_directory_path}bitstream_listen.py"
mpr_ls = f"{mpr_connect} fs ls /"
mpr_rm = f"{mpr_connect} fs rm "

def exec(cmd, op_file_path="null"):
    if cmd == mpr_rm:
        # Check if file is present already
        status = os.system(f"{mpr_ls} | grep {op_file_path}")
        # If the file is present, remove it
        if status == 0:
            os.system(f"{cmd} {op_file_path}")
    else:
        with open(op_file_path, "a") as file:
            status = os.system(cmd)
            if status != 0:
                print("error")


#bistream generation
def instance0(): 
    exec(mpr_rm, "bitstream_listen.py") # Remove file if it exists
    exec(mpr_file_cp) # Copy the file to the remote device
    exec(mpr_execute) # Execute the script remotely
    


#bitstream capture
def instance1():
    print("bitstream")
    p0 = Pin('P13_6', Pin.OUT, value=0)
    bitstream(p0, 0, timing, buf)
