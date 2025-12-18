# Test corner cases where MicroPython and CPython outputs used to differ in the past
print("%.1g" % -9.9)  # round up 'g' with '-' sign
print("%.2g" % 99.9)  # round up
