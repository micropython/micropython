include('boards/manifest.py')

LIB = '../../../micropython-lib'

freeze(LIB + '/upysh', 'upysh.py')
freeze(LIB + '/urequests', 'urequests.py')
freeze(LIB + '/umqtt.simple', 'umqtt/simple.py')
freeze(LIB + '/umqtt.robust', 'umqtt/robust.py')
