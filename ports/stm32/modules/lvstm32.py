# Module for advancing tick count and scheduling async event for lvgl on STM32.
# Import after lvgl module.
#
# MIT license; Copyright (c) 2020 Amir Gonnen

import lvgl as lv
import micropython
import pyb

class lvstm32():
    def __init__(self, freq=25, timer_id=4):
        self.task_handler_ref = self.task_handler  # Allocation occurs here
        self.delay = 1000 // freq
        tim = pyb.Timer(timer_id)
        tim.init(freq=freq)
        tim.callback(self.timer_cb)

    def task_handler(self, _):
        lv.task_handler()

    def timer_cb(self, t):
        lv.tick_inc(self.delay)
        # Passing self.task_handler would cause allocation.
        micropython.schedule(self.task_handler_ref, 0)

