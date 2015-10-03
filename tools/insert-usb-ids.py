# Reads the USB VID and PID from the file specifed by sys.arg[1] and then
# inserts those values into the template file specified by sys.argv[2],
# printing the result to stdout

from __future__ import print_function

import sys
import re
import string

needed_keys = ('USB_PID_CDC_MSC', 'USB_PID_CDC_HID', 'USB_PID_CDC', 'USB_VID')

def parse_usb_ids(filename):
    rv = dict()
    for line in open(filename).readlines():
        line = line.rstrip('\r\n')
        match = re.match('^#define\s+(\w+)\s+\(0x([0-9A-Fa-f]+)\)$', line)
        if match and match.group(1).startswith('USBD_'):
            key = match.group(1).replace('USBD', 'USB')
            val = match.group(2)
            print("key =", key, "val =", val)
            if key in needed_keys:
                rv[key] = val
    for k in needed_keys:
        if k not in rv:
            raise Exception("Unable to parse %s from %s" % (k, filename))
    return rv

if __name__ == "__main__":
    usb_ids_file = sys.argv[1]
    template_file = sys.argv[2]
    replacements = parse_usb_ids(usb_ids_file)
    for line in open(template_file, 'r').readlines():
        print(string.Template(line).safe_substitute(replacements), end='')
