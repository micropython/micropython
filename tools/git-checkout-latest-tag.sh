#!/bin/bash

set -eo pipefail

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

git fetch --tags
latest_tag="$(git describe --tags `git rev-list --tags --max-count=1`)"
git checkout $latest_tag
