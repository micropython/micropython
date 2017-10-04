# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2017 Glenn Ruben Bakke
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE

from pyb import LED
from machine import RTC, Temp
from ubluepy import Service, Characteristic, UUID, Peripheral, constants

def event_handler(id, handle, data):
    global rtc
    global periph
    global serv_env_sense
    global notif_enabled
 
    if id == constants.EVT_GAP_CONNECTED:
        # indicated 'connected'
        LED(1).on()

    elif id == constants.EVT_GAP_DISCONNECTED:
        # stop low power timer
        rtc.stop()
        # indicate 'disconnected'
        LED(1).off()
        # restart advertisment
        periph.advertise(device_name="micr_temp", services=[serv_env_sense])

    elif id == constants.EVT_GATTS_WRITE:
        # write to this Characteristic is to CCCD
        if int(data[0]) == 1:
            notif_enabled = True
            # start low power timer
            rtc.start()
        else: 
            notif_enabled = False
            # stop low power timer
            rtc.stop()

def send_temp(timer_id):
    global notif_enabled
    global char_temp

    if notif_enabled:
        # measure chip temperature
        temp = Temp.read()
        temp =  temp * 100
        char_temp.write(bytearray([temp & 0xFF, temp >> 8]))

# start off with LED(1) off
LED(1).off()

# use RTC1 as RTC0 is used by bluetooth stack 
# set up RTC callback every 5 second
rtc = RTC(1, period=5, mode=RTC.PERIODIC, callback=send_temp)

notif_enabled = False

uuid_env_sense = UUID("0x181A") # Environmental Sensing service
uuid_temp = UUID("0x2A6E") # Temperature characteristic
 
serv_env_sense = Service(uuid_env_sense)

temp_props = Characteristic.PROP_NOTIFY | Characteristic.PROP_READ
temp_attrs = Characteristic.ATTR_CCCD
char_temp = Characteristic(uuid_temp, props = temp_props, attrs = temp_attrs)

serv_env_sense.addCharacteristic(char_temp)

periph = Peripheral()
periph.addService(serv_env_sense)
periph.setConnectionHandler(event_handler)
periph.advertise(device_name="micr_temp", services=[serv_env_sense])

