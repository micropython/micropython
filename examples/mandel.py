try:
    import micropython
except:
    pass

def mandelbrot():
    # returns True if c, complex, is in the Mandelbrot set
    #@micropython.native
    def in_set(c):
        z = 0
        for i in range(40):
            z = z*z + c
            if abs(z) > 60:
                return False
        return True

    lcd.clear()
    for u in range(91):
        for v in range(31):
            if in_set((u / 30 - 2) + (v / 15 - 1) * 1j):
                lcd.set(u, v)
    lcd.show()

# PC testing
import lcd
lcd = lcd.LCD(128, 32)
mandelbrot()
