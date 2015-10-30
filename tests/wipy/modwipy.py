'''
wipy module test for the CC3200 based boards
'''

import os
import wipy

mch = os.uname().machine
if not 'LaunchPad' in mch and not'WiPy' in mch:
    raise Exception('Board not supported!')

print(wipy.heartbeat() == True)
wipy.heartbeat(False)
print(wipy.heartbeat() == False)
wipy.heartbeat(True)
print(wipy.heartbeat() == True)

try:
    wipy.heartbeat(True, 1)
except:
    print('Exception')
