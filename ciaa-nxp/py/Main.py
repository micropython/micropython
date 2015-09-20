import pyb

def func(sw):
        print("sw pressed!")
        print(sw)

switch1 = pyb.Switch(1)
switch1.callback(func)
while True:
        pyb.delay(1000)
