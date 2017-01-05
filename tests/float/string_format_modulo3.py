# uPy and CPython outputs differ for the following
print("%.1g" % -9.9) # round up 'g' with '-' sign
print("%.1e" % 9.99) # round up with positive exponent
print("%.1e" % 0.999) # round up with negative exponent
