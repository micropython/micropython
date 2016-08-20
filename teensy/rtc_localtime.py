#!/usr/bin/env python

from __future__ import print_function

import calendar
import time

# Get the localtime and then convert it back to seconds. This will give us a
# time compatible with what Arduino generates

print(int(calendar.timegm(time.localtime())))
