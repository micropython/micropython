#### SoftI2C
import os
from machine import I2S
import time
import array

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    sck_tx_pin = "P13_1"
    ws_tx_pin = "P13_2"
    sd_tx_pin = "P13_3"
    sck_rx_pin = "P5_4"
    ws_rx_pin = "P5_5"
    sd_rx_pin = "P5_6"

audio_out = I2S(
    0,
    sck=sck_tx_pin,
    ws=ws_tx_pin,
    sd=sd_tx_pin,
    mode=I2S.TX,
    bits=16,
    format=I2S.STEREO,
    rate=8000,
    ibuf=20000,
)
audio_in = I2S(
    1,
    sck=sck_rx_pin,
    ws=ws_rx_pin,
    sd=sd_rx_pin,
    mode=I2S.RX,
    bits=16,
    format=I2S.STEREO,
    rate=8000,
    ibuf=20000,
)
buf = bytearray(10)
buf = b"\x01\x00\x17\x15\x16\x44"
print("tx Buffer")
print(buf)
num_written = audio_out.write(buf)
buf1 = bytearray(10)
num_read = audio_in.readinto(buf1)
print("Rx Buffer")
print(buf1)
