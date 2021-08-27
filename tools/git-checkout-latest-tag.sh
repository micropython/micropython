#!/bin/bash

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

git fetch --tags
git checkout $(git describe --tags `git rev-list --tags --max-count=1`)
