# This is hwconfig for "emulation" for cases when there's no real hardware.
# It just prints information to console.
class LEDClass:

    def __init__(self, id):
        self.id = id

    def value(self, v):
        print("LED(%d):" % self.id, v)


LED = LEDClass(1)
LED2 = LEDClass(12)
