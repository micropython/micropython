print("%s" % 1.0)
print("%r" % 1.0)

print("%d" % 1.0)
print("%i" % 1.0)
print("%u" % 1.0)

# these 3 have different behaviour in Python 3.x versions
# uPy raises a TypeError, following Python 3.5 (earlier versions don't)
#print("%x" % 18.0)
#print("%o" % 18.0)
#print("%X" % 18.0)

print("%e" % 1.23456)
print("%E" % 1.23456)
print("%f" % 1.23456)
print("%F" % 1.23456)
print("%g" % 1.23456)
print("%G" % 1.23456)
