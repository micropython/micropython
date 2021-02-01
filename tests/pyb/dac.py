import pyb

if not hasattr(pyb, "DAC"):
    print("SKIP")
    raise SystemExit

dac = pyb.DAC(1)
print(dac)
dac.noise(100)
dac.triangle(100)
dac.write(0)
dac.write_timed(bytearray(10), 100, mode=pyb.DAC.NORMAL)
pyb.delay(20)
dac.write(0)

# test buffering arg
dac = pyb.DAC(1, buffering=True)
dac.write(0)
