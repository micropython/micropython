from pyb import Switch

sw = Switch()
print(sw())
sw.callback(print)
sw.callback(None)
