import pyb
from pyb import Timer

tim = Timer(4)
tim = Timer(4, prescaler=100, period=200)
print(tim.prescaler())
print(tim.period())
tim.prescaler(300)
print(tim.prescaler())
tim.period(400)
print(tim.period())

tim = Timer(4, freq=1)
tim.init(freq=2000)
def f(t):
    print(1)
    t.callback(None)
tim.callback(f)
pyb.delay(10)

# f3 closes over f2.y
def f2(x):
    y = x
    def f3(t):
        print(2, y)
        t.callback(None)
    return f3
tim.callback(f2(3))
pyb.delay(10)
