@micropython.native
def f(led, n):
    led.off()
    i = 0
    while i < n:
        led.toggle()
        d = pyb.delay
        d(50) # pyb.delay(50) doesn't work!
        i += 1
        print(i)
    led.off()

f(pyb.LED(1), 2)
f(pyb.LED(2), 4)
