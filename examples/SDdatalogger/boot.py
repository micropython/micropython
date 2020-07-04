# boot.py -- runs on boot-up
# Let's you choose which script to run.
# > To run 'datalogger.py':
#       * press reset and do nothing else
# > To run 'cardreader.py':
#       * press reset
#       * press user switch and hold until orange LED goes out

import pyb

pyb.LED(3).on()  # indicate we are waiting for switch press
pyb.delay(2000)  # wait for user to maybe press the switch
switch_value = pyb.Switch()()  # sample the switch at end of delay
pyb.LED(3).off()  # indicate that we finished waiting for the switch

pyb.LED(4).on()  # indicate that we are selecting the mode

if switch_value:
    pyb.usb_mode("VCP+MSC")
    pyb.main("cardreader.py")  # if switch was pressed, run this
else:
    pyb.usb_mode("VCP+HID")
    pyb.main("datalogger.py")  # if switch wasn't pressed, run this

pyb.LED(4).off()  # indicate that we finished selecting the mode
