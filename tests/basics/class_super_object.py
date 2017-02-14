# Calling object.__init__() via super().__init__
try:
    # If we don't expose object.__init__ (small ports), there's
    # nothing to test.
    object.__init__
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()

class Test(object):
    def __init__(self):
        super().__init__()
        print("Test.__init__")

t = Test()

class Test2:
    def __init__(self):
        super().__init__()
        print("Test2.__init__")

t = Test2()
