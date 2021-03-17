print("%s" % 1.0)
print("%r" % 1.0)

print("%d" % 1.0)
print("%i" % 1.0)
print("%u" % 1.0)

# these 3 have different behaviour in Python 3.x versions
# uPy raises a TypeError, following Python 3.5 (earlier versions don't)
# print("%x" % 18.0)
# print("%o" % 18.0)
# print("%X" % 18.0)

print("%e" % 1.23456)
print("%E" % 1.23456)
print("%f" % 1.23456)
print("%F" % 1.23456)
print("%g" % 1.23456)
print("%G" % 1.23456)

print("%06e" % float("inf"))
print("%06e" % float("-inf"))
print("%06e" % float("nan"))

print("%02.3d" % 123)  # prec > width
print("%+f %+f" % (1.23, -1.23))  # float sign
print("% f % f" % (1.23, -1.23))  # float space sign
print("%0f" % -1.23)  # negative number with 0 padding

# numbers with large negative exponents
print("%f" % 1e-10)
print("%f" % 1e-20)
print("%f" % 1e-50)
print("%f" % 1e-100)
print("%f" % 1e-300)

# large decimal precision should be truncated and not overflow buffer
# the output depends on the FP calculation so only first 2 digits are printed
# (the 'g' with small e are printed using 'f' style, so need to be checked)
print(("%.40f" % 1e-300)[:2])
print(("%.40g" % 1e-1)[:2])
print(("%.40g" % 1e-2)[:2])
print(("%.40g" % 1e-3)[:2])
print(("%.40g" % 1e-4)[:2])

print("%.0g" % 1)  # 0 precision 'g'

print("%.1e" % 9.99)  # round up with positive exponent
print("%.1e" % 0.999)  # round up with negative exponent
