#!/usr/bin/env python

"""
pyboard interface

This module provides the Pyboard class, used to communicate with and
control the pyboard over a serial USB connection.

Example usage:

    import pyboard
    pyb = pyboard.Pyboard('/dev/ttyACM0')
    pyb.enter_raw_repl()
    pyb.exec('pyb.LED(1).on()')
    pyb.exit_raw_repl()

To run a script from the local machine on the board and print out the results:

    import pyboard
    pyboard.execfile('test.py', device='/dev/ttyACM0')

This script can also be run directly.  To execute a local script, use:

    ./pyboard.py test.py

Or:

    python pyboard.py test.py

"""

import sys
import time
import serial

def stdout_write_bytes(b):
    sys.stdout.buffer.write(b)
    sys.stdout.buffer.flush()

class PyboardError(BaseException):
    pass

class Pyboard:
    def __init__(self, serial_device, baudrate=115200):
        self.serial = serial.Serial(serial_device, baudrate=baudrate, interCharTimeout=1)

    def close(self):
        self.serial.close()

    def read_until(self, min_num_bytes, ending, timeout=10, data_consumer=None):
        data = self.serial.read(min_num_bytes)
        if data_consumer:
            data_consumer(data)
        timeout_count = 0
        while True:
            if data.endswith(ending):
                break
            elif self.serial.inWaiting() > 0:
                new_data = self.serial.read(1)
                data = data + new_data
                if data_consumer:
                    data_consumer(new_data)
                #time.sleep(0.01)
                timeout_count = 0
            else:
                timeout_count += 1
                if timeout is not None and timeout_count >= 10 * timeout:
                    break
                time.sleep(0.1)
        return data

    def enter_raw_repl(self):
        self.serial.write(b'\r\x03\x03') # ctrl-C twice: interrupt any running program
        self.serial.write(b'\r\x01') # ctrl-A: enter raw REPL
        data = self.read_until(1, b'to exit\r\n>')
        if not data.endswith(b'raw REPL; CTRL-B to exit\r\n>'):
            print(data)
            raise PyboardError('could not enter raw repl')
        self.serial.write(b'\x04') # ctrl-D: soft reset
        data = self.read_until(1, b'to exit\r\n>')
        if not data.endswith(b'raw REPL; CTRL-B to exit\r\n>'):
            print(data)
            raise PyboardError('could not enter raw repl')

    def exit_raw_repl(self):
        self.serial.write(b'\r\x02') # ctrl-B: enter friendly REPL

    def follow(self, timeout, data_consumer=None):
        # wait for normal output
        data = self.read_until(1, b'\x04', timeout=timeout, data_consumer=data_consumer)
        if not data.endswith(b'\x04'):
            raise PyboardError('timeout waiting for first EOF reception')
        data = data[:-1]

        # wait for error output
        data_err = self.read_until(2, b'\x04>', timeout=timeout)
        if not data_err.endswith(b'\x04>'):
            raise PyboardError('timeout waiting for second EOF reception')
        data_err = data_err[:-2]

        # return normal and error output
        return data, data_err

    def exec_raw_no_follow(self, command):
        if isinstance(command, bytes):
            command_bytes = command
        else:
            command_bytes = bytes(command, encoding='utf8')

        # write command
        for i in range(0, len(command_bytes), 256):
            self.serial.write(command_bytes[i:min(i + 256, len(command_bytes))])
            time.sleep(0.01)
        self.serial.write(b'\x04')

        # check if we could exec command
        data = self.serial.read(2)
        if data != b'OK':
            raise PyboardError('could not exec command')

    def exec_raw(self, command, timeout=10, data_consumer=None):
        self.exec_raw_no_follow(command);
        return self.follow(timeout, data_consumer)

    def eval(self, expression):
        ret = self.exec('print({})'.format(expression))
        ret = ret.strip()
        return ret

    def exec(self, command):
        ret, ret_err = self.exec_raw(command)
        if ret_err:
            raise PyboardError('exception', ret, ret_err)
        return ret

    def execfile(self, filename):
        with open(filename, 'rb') as f:
            pyfile = f.read()
        return self.exec(pyfile)

    def get_time(self):
        t = str(self.eval('pyb.RTC().datetime()'), encoding='utf8')[1:-1].split(', ')
        return int(t[4]) * 3600 + int(t[5]) * 60 + int(t[6])

def execfile(filename, device='/dev/ttyACM0'):
    pyb = Pyboard(device)
    pyb.enter_raw_repl()
    output = pyb.execfile(filename)
    stdout_write_bytes(output)
    pyb.exit_raw_repl()
    pyb.close()

def run_test(device):
    pyb = Pyboard(device)
    pyb.enter_raw_repl()
    print('opened device {}'.format(device))

    pyb.exec('import pyb')  # module pyb no longer imported by default, required for pyboard tests
    print('seconds since boot:', pyb.get_time())

    pyb.exec('def apply(l, f):\r\n for item in l:\r\n  f(item)\r\n')

    pyb.exec('leds=[pyb.LED(l) for l in range(1, 5)]')
    pyb.exec('apply(leds, lambda l:l.off())')

    ## USR switch test

    pyb.exec('switch = pyb.Switch()')

    for i in range(2):
        print("press USR button")
        pyb.exec('while switch(): pyb.delay(10)')
        pyb.exec('while not switch(): pyb.delay(10)')

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

def main():
    import argparse
    cmd_parser = argparse.ArgumentParser(description='Run scripts on the pyboard.')
    cmd_parser.add_argument('--device', default='/dev/ttyACM0', help='the serial device of the pyboard')
    cmd_parser.add_argument('--follow', action='store_true', help='follow the output after running the scripts [default if no scripts given]')
    cmd_parser.add_argument('--test', action='store_true', help='run a small test suite on the pyboard')
    cmd_parser.add_argument('files', nargs='*', help='input files')
    args = cmd_parser.parse_args()

    if args.test:
        run_test(device=args.device)

    for filename in args.files:
        try:
            pyb = Pyboard(args.device)
            pyb.enter_raw_repl()
            with open(filename, 'rb') as f:
                pyfile = f.read()
            ret, ret_err = pyb.exec_raw(pyfile, timeout=None, data_consumer=stdout_write_bytes)
            pyb.exit_raw_repl()
            pyb.close()
        except PyboardError as er:
            print(er)
            sys.exit(1)
        except KeyboardInterrupt:
            sys.exit(1)
        if ret_err:
            stdout_write_bytes(ret_err)
            sys.exit(1)

    if args.follow or len(args.files) == 0:
        try:
            pyb = Pyboard(args.device)
            ret, ret_err = pyb.follow(timeout=None, data_consumer=stdout_write_bytes)
            pyb.close()
        except PyboardError as er:
            print(er)
            sys.exit(1)
        except KeyboardInterrupt:
            sys.exit(1)
        if ret_err:
            stdout_write_bytes(ret_err)
            sys.exit(1)

if __name__ == "__main__":
    main()
