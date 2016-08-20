#!/usr/bin/env python

from __future__ import print_function

import csv

pins = []
line_len = 0

with open('mk66_af.csv', 'r') as csvfile:
    for row in csv.reader(csvfile):
        if len(row) != line_len:
            if line_len:
                print('Row[', row[0], '] len =', line_len)
            else:
                line_len = len(row)
        for col in row[2:]:
            if col:
                for pin in col.split('/'):
                    pins.append(pin)

for pin in sorted(pins):
    print(pin)
