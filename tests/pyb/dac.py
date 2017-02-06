import pyb

if not hasattr(pyb, 'DAC'):
    print('SKIP')
    import sys
    sys.exit()

dac = pyb.DAC(1)
print(dac)
dac.noise(100)
dac.triangle(100)
dac.write(0)
dac.write_timed(bytearray(10), 100, mode=pyb.DAC.NORMAL)
pyb.delay(20)
dac.write(0)
