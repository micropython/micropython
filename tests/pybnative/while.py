import pyb

@micropython.native
def f(led, n, d):
    led.off()
    i = 0
    while i < n:
        print(i)
        led.toggle()
        pyb.delay(d)
        i += 1
    led.off()

f(pyb.LED(1), 2, 150)
f(pyb.LED(2), 4, 50)
