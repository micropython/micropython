# boot.py -- runs on boot-up
# Let's you choose which script to run.
# > To run 'datalogger.py':
#       * press reset and do nothing else
# > To run 'cardreader.py':
#       * press reset
#       * press user switch and hold until orange LED goes out

import pyb

pyb.LED(3).on()
pyb.delay(2000)
pyb.LED(4).on()
pyb.LED(3).off()
switch = pyb.Switch()                   # check if switch was pressed decision phase

if switch():
    pyb.usb_mode('CDC+MSC')
    pyb.main('cardreader.py')           # if switch was pressed, run this
else:
    pyb.usb_mode('CDC+HID')
    pyb.main('datalogger.py')           # if switch wasn't pressed, run this

pyb.LED(4).off()
