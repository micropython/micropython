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

# MicroPython controller for PowerUp 3.0 paper airplane
# https://www.poweruptoys.com/products/powerup-v3
#
# Examples is written for nrf52832, pca10040 using s132 bluetooth stack.
#
# Joystick shield pin mapping:
#  - analog stick x-direction - ADC0 - P0.02/"P2"
#  - buttons P0.13 - P0.16 / "P13", "P14", "P15", "P16"
#
# Example usage:
#
#     from powerup import PowerUp3
#     p = PowerUp3()

import time
from machine import ADC
from machine import Pin
from ubluepy import Peripheral, Scanner, constants


def bytes_to_str(bytes):
    string = ""
    for b in bytes:
        string += chr(b)
    return string


def get_device_names(scan_entries):
    dev_names = []
    for e in scan_entries:
        scan = e.getScanData()
        if scan:
            for s in scan:
                if s[0] == constants.ad_types.AD_TYPE_COMPLETE_LOCAL_NAME:
                    dev_names.append((e, bytes_to_str(s[2])))
    return dev_names


def find_device_by_name(name):
    s = Scanner()
    scan_res = s.scan(500)

    device_names = get_device_names(scan_res)
    for dev in device_names:
        if name == dev[1]:
            return dev[0]


class PowerUp3:
    def __init__(self):
        self.x_adc = ADC(1)

        self.btn_speed_up = Pin("P13", mode=Pin.IN, pull=Pin.PULL_UP)
        self.btn_speed_down = Pin("P15", mode=Pin.IN, pull=Pin.PULL_UP)
        self.btn_speed_full = Pin("P14", mode=Pin.IN, pull=Pin.PULL_UP)
        self.btn_speed_off = Pin("P16", mode=Pin.IN, pull=Pin.PULL_UP)

        self.x_mid = 0

        self.calibrate()
        self.connect()
        self.loop()

    def read_stick_x(self):
        return self.x_adc.value()

    def button_speed_up(self):
        return not bool(self.btn_speed_up.value())

    def button_speed_down(self):
        return not bool(self.btn_speed_down.value())

    def button_speed_full(self):
        return not bool(self.btn_speed_full.value())

    def button_speed_off(self):
        return not bool(self.btn_speed_off.value())

    def calibrate(self):
        self.x_mid = self.read_stick_x()

    def __str__(self):
        return "calibration x: %i, y: %i" % (self.x_mid)

    def map_chars(self):
        s = self.p.getServices()

        service_batt = s[3]
        service_control = s[4]

        self.char_batt_lvl = service_batt.getCharacteristics()[0]
        self.char_control_speed = service_control.getCharacteristics()[0]
        self.char_control_angle = service_control.getCharacteristics()[2]

    def battery_level(self):
        return int(self.char_batt_lvl.read()[0])

    def speed(self, new_speed=None):
        if new_speed == None:
            return int(self.char_control_speed.read()[0])
        else:
            self.char_control_speed.write(bytearray([new_speed]))

    def angle(self, new_angle=None):
        if new_angle == None:
            return int(self.char_control_angle.read()[0])
        else:
            self.char_control_angle.write(bytearray([new_angle]))

    def connect(self):
        dev = None

        # connect to the airplane
        while not dev:
            dev = find_device_by_name("TailorToys PowerUp")
            if dev:
                self.p = Peripheral()
                self.p.connect(dev.addr())

        # locate interesting characteristics
        self.map_chars()

    def rudder_center(self):
        if self.old_angle != 0:
            self.old_angle = 0
            self.angle(0)

    def rudder_left(self, angle):
        steps = angle // self.interval_size_left
        new_angle = 60 - steps

        if self.old_angle != new_angle:
            self.angle(new_angle)
            self.old_angle = new_angle

    def rudder_right(self, angle):
        steps = angle // self.interval_size_right
        new_angle = -steps

        if self.old_angle != new_angle:
            self.angle(new_angle)
            self.old_angle = new_angle

    def throttle(self, speed):
        if speed > 200:
            speed = 200
        elif speed < 0:
            speed = 0

        if self.old_speed != speed:
            self.speed(speed)
            self.old_speed = speed

    def loop(self):
        adc_threshold = 10
        right_threshold = self.x_mid + adc_threshold
        left_threshold = self.x_mid - adc_threshold

        self.interval_size_left = self.x_mid // 60
        self.interval_size_right = (255 - self.x_mid) // 60

        self.old_angle = 0
        self.old_speed = 0

        while True:

            time.sleep_ms(100)

            # read out new angle
            new_angle = self.read_stick_x()
            if new_angle < 256:
                if new_angle > right_threshold:
                    self.rudder_right(new_angle - self.x_mid)
                elif new_angle < left_threshold:
                    self.rudder_left(new_angle)
                else:
                    self.rudder_center()

            # read out new speed
            new_speed = self.old_speed

            if self.button_speed_up():
                new_speed += 25
            elif self.button_speed_down():
                new_speed -= 25
            elif self.button_speed_full():
                new_speed = 200
            elif self.button_speed_off():
                new_speed = 0
            else:
                pass

            self.throttle(new_speed)
