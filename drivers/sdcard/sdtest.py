# Test for sdcard block protocol
# Peter hinch 30th Jan 2016
import os, sdcard, machine

def sdtest():
    spi = machine.SPI(1)
    spi.init()  # Ensure right baudrate
    sd = sdcard.SDCard(spi, machine.Pin.board.X21) # Compatible with PCB
    vfs = os.VfsFat(sd)
    os.mount(vfs, '/fc')
    print('Filesystem check')
    print(os.listdir('/fc'))

    line = 'abcdefghijklmnopqrstuvwxyz\n'
    lines = line * 200 # 5400 chars
    short = '1234567890\n'

    fn = '/fc/rats.txt'
    print()
    print('Multiple block read/write')
    with open(fn,'w') as f:
        n = f.write(lines)
        print(n, 'bytes written')
        n = f.write(short)
        print(n, 'bytes written')
        n = f.write(lines)
        print(n, 'bytes written')

    with open(fn,'r') as f:
        result1 = f.read()
        print(len(result1), 'bytes read')

    fn = '/fc/rats1.txt'
    print()
    print('Single block read/write')
    with open(fn,'w') as f:
        n = f.write(short) # one block
        print(n, 'bytes written')

    with open(fn,'r') as f:
        result2 = f.read()
        print(len(result2), 'bytes read')

    os.umount('/fc')

    print()
    print('Verifying data read back')
    success = True
    if result1 == ''.join((lines, short, lines)):
        print('Large file Pass')
    else:
        print('Large file Fail')
        success = False
    if result2 == short:
        print('Small file Pass')
    else:
        print('Small file Fail')
        success = False
    print()
    print('Tests', 'passed' if success else 'failed')
