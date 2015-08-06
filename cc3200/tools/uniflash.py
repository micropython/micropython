#!/usr/bin/env python

"""
Flash the WiPy (format, update service pack and program).

Example:

> python uniflash.py -u "C:\ti\uniflash_3.2\uniflashCLI.bat" -c "C:\VirtualBoxShared\GitHub\wipy_uniflash.usf" -p 8 -s "C:\ti\CC31xx_CC32xx_ServicePack_1.0.0.10.0\servicepack_1.0.0.10.0.bin"

or:

> python uniflash.py -u "C:\ti\uniflash_3.2\uniflashCLI.bat" -c "C:\VirtualBoxShared\GitHub\launchxl_uniflash.usf" -p 8 -s "C:\ti\CC31xx_CC32xx_ServicePack_1.0.0.10.0\servicepack_1.0.0.10.0.bin"

"""

import sys
import argparse
import subprocess


def print_exception(e):
    print ('Exception: {}, on line {}'.format(e, sys.exc_info()[-1].tb_lineno))


def main():
    cmd_parser = argparse.ArgumentParser(description='Flash the WiPy and optionally run a small test on it.')
    cmd_parser.add_argument('-u', '--uniflash', default=None, help='the path to the uniflash cli executable')
    cmd_parser.add_argument('-c', '--config', default=None, help='the path to the uniflash config file')
    cmd_parser.add_argument('-p', '--port', default=8, help='the com serial port')
    cmd_parser.add_argument('-s', '--servicepack', default=None, help='the path to the servicepack file')
    args = cmd_parser.parse_args()

    result = 1
    com_port = 'com=' + str(args.port)
    servicepack_path = 'spPath=' + args.servicepack

    try:
        if args.uniflash == None or args.config == None:
            raise ValueError('uniflash path and config path are mandatory')
        if args.servicepack == None:
            subprocess.check_call([args.uniflash, '-config', args.config, '-setOptions', com_port, '-operations', 'format', 'program'], stderr=subprocess.STDOUT)
        else:
            subprocess.check_call([args.uniflash, '-config', args.config, '-setOptions', com_port, servicepack_path, '-operations', 'format', 'servicePackUpdate', 'program'], stderr=subprocess.STDOUT)
        result = 0
    except Exception as e:
        print_exception(e)
    finally:
        if result:
            print ("ERROR: Programming failed!")
        else:
            print ("Board programmed OK")
        sys.exit(result)


if __name__ == "__main__":
    main()
