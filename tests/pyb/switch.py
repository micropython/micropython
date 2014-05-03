from pyb import Switch

sw = pyb.Switch()
print(sw())
sw.callback(print)
sw.callback(None)
