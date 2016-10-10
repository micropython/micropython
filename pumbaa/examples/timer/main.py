# 
# @file main.py
# 
# @section License
# Copyright (C) 2016, Erik Moqvist
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# This file is part of the Pumbaa project.
# 

from kernel import Timer
from sync import Event

EVENT = Event()

TIMER = Timer(1, EVENT, 0x1, flags=Timer.PERIODIC)
TIMER.start()

for i in range(10):
    EVENT.read(0x1)
    print("timeout", i)

TIMER.stop()
