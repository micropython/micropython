# Reads the USB VID and PID from the file specifed by sys.arg[1] and then
# inserts those values into the template file specified by sys.argv[2],
# printing the result to stdout

from __future__ import print_function

import sys
import re
import string

def parse_usb_ids(filename):
    rv = dict()
    if filename == 'usbd_desc.c':
        for line in open(filename).readlines():
            line = line.rstrip('\r\n')
            match = re.match('^#define\s+(\w+)\s+0x([0-9A-Fa-f]+)$', line)
            if match:
                if match.group(1) == 'USBD_VID':
                    rv['USB_VID'] = match.group(2)
                elif match.group(1) == 'USBD_PID':
                    rv['USB_PID'] = match.group(2)
                if 'USB_VID' in rv and 'USB_PID' in rv:
                    break
    else:
        raise Exception("Don't (yet) know how to parse USB IDs from %s" % filename)
    for k in ('USB_PID', 'USB_VID'):
        if k not in rv:
            raise Exception("Unable to parse %s from %s" % (k, filename))
    return rv

if __name__ == "__main__":
    usb_ids_file = sys.argv[1]
    template_file = sys.argv[2]
    replacements = parse_usb_ids(usb_ids_file)
    for line in open(template_file, 'r').readlines():
        print(string.Template(line).safe_substitute(replacements), end='')
