'''
network server test for the CC3200 based boards.
'''

import os
import network

mch = os.uname().machine
if not 'LaunchPad' in mch and not'WiPy' in mch:
    raise Exception('Board not supported!')

server = network.Server()

print(server.timeout() == 300)
print(server.isrunning() == True)
server.deinit()
print(server.isrunning() == False)

server.init(login=('test-user', 'test-password'), timeout=60)
print(server.isrunning() == True)
print(server.timeout() == 60)

server.deinit()
print(server.isrunning() == False)
server.init()
print(server.isrunning() == True)

try:
    server.init(1)
except:
    print('Exception')

try:
    server.init(0, login=('0000000000011111111111222222222222333333', 'abc'))
except:
    print('Exception')

try:
    server.timeout(1)
except:
    print('Exception')
