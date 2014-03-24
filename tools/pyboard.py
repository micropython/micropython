"""
pyboard interface

This module provides the Pyboard class, used to communicate with and
control the pyboard over a serial USB connection.

Example usage:

    import pyboard
    pyb = pyboard.Pyboard('/dev/ttyACM0')
    pyb.enter_raw_repl()
    pyb.exec('pyb.Led(1).on()')
    pyb.exit_raw_repl()

"""

import time
import serial

class Pyboard:
    def __init__(self, serial_device):
        self.serial = serial.Serial(serial_device)

    def close(self):
        self.serial.close()

    def enter_raw_repl(self):
        self.serial.write(b'\r\x01') # ctrl-A: enter raw REPL
        self.serial.write(b'\x04') # ctrl-D: soft reset
        data = self.serial.read(1)
        while self.serial.inWaiting() > 0:
            data = data + self.serial.read(self.serial.inWaiting())
            time.sleep(0.1)
        if not data.endswith(b'raw REPL; CTRL-B to exit\r\n>'):
            print(data)
            raise Exception('could not enter raw repl')

    def exit_raw_repl(self):
        self.serial.write(b'\r\x02') # ctrl-B: enter friendly REPL

    def eval(self, expression):
        ret = self.exec('print({})'.format(expression))
        ret = ret.strip()
        return ret

    def exec(self, command):
        command_bytes = bytes(command, encoding='ascii')
        for i in range(0, len(command_bytes), 10):
            self.serial.write(command_bytes[i:min(i+10, len(command_bytes))])
            time.sleep(0.01)
        self.serial.write(b'\x04')
        data = self.serial.read(2)
        if data != b'OK':
            raise Exception('could not exec command')
        data = self.serial.read(2)
        while self.serial.inWaiting() > 0:
            data = data + self.serial.read(self.serial.inWaiting())
            time.sleep(0.1)
        if not data.endswith(b'\x04>'):
            print(data)
            raise Exception('could not exec command')
        if data.startswith(b'Traceback') or data.startswith(b'  File '):
            print(data)
            raise Exception('command failed')
        return data[:-2]

    def get_time(self):
        t = str(self.exec('pyb.time()'), encoding='ascii').strip().split()[1].split(':')
        return int(t[0]) * 3600 + int(t[1]) * 60 + int(t[2])

def run_test():
    device = '/dev/ttyACM0'
    pyb = Pyboard(device)
    pyb.enter_raw_repl()
    print('opened device {}'.format(device))

    print('seconds since boot:', pyb.get_time())

    pyb.exec('def apply(l, f):\r\n for item in l:\r\n  f(item)\r\n')

    pyb.exec('leds=[pyb.Led(l) for l in range(1, 5)]')
    pyb.exec('apply(leds, lambda l:l.off())')

    ## USR switch test

    if True:
        for i in range(2):
            print("press USR button")
            pyb.exec('while pyb.switch(): pyb.delay(10)')
            pyb.exec('while not pyb.switch(): pyb.delay(10)')

        print('USR switch passed')

    ## accel test

    if True:
        print("hold level")
        pyb.exec('accel = pyb.Accel()')
        pyb.exec('while abs(accel.x()) > 10 or abs(accel.y()) > 10: pyb.delay(10)')

        print("tilt left")
        pyb.exec('while accel.x() > -10: pyb.delay(10)')
        pyb.exec('leds[0].on()')

        print("tilt forward")
        pyb.exec('while accel.y() < 10: pyb.delay(10)')
        pyb.exec('leds[1].on()')

        print("tilt right")
        pyb.exec('while accel.x() < 10: pyb.delay(10)')
        pyb.exec('leds[2].on()')

        print("tilt backward")
        pyb.exec('while accel.y() > -10: pyb.delay(10)')
        pyb.exec('leds[3].on()')

        print('accel passed')

    print('seconds since boot:', pyb.get_time())

    pyb.exec('apply(leds, lambda l:l.off())')

    pyb.exit_raw_repl()
    pyb.close()

if __name__ == "__main__":
    run_test()
