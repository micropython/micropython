# Calling object.__init__() via super().__init__

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
