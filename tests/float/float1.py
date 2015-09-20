# test basic float capabilities

# literals
print(.12)
print(1.)
print(1.2)
print(0e0)
print(0e+0)
print(0e-0)

# float construction
print(float(1.2))
print(float("1.2"))
print(float("+1"))
print(float("1e1"))
print(float("1e+1"))
print(float("1e-1"))
print(float("inf"))
print(float("-inf"))
print(float("INF"))
print(float("infinity"))
print(float("INFINITY"))
print(float("nan"))
print(float("NaN"))
try:
    float("1e+")
except ValueError:
    print("ValueError")
try:
    float("1z")
except ValueError:
    print("ValueError")

# unary operators
print(bool(0.0))
print(bool(1.2))
print(+(1.2))
print(-(1.2))

# division of integers
x = 1 / 2
print(x)

# /= operator
a = 1
a /= 2
print(a)

# floor division
print(1.0 // 2)
print(2.0 // 2)

# comparison
print(1.2 <= 3.4)
print(1.2 <= -3.4)
print(1.2 >= 3.4)
print(1.2 >= -3.4)

try:
    1.0 / 0
except ZeroDivisionError:
    print("ZeroDivisionError")

try:
    1.0 // 0
except ZeroDivisionError:
    print("ZeroDivisionError")

try:
    1.2 % 0
except ZeroDivisionError:
    print("ZeroDivisionError")

# unsupported unary ops

try:
    ~1.2
except TypeError:
    print("TypeError")

try:
    1.2 in 3.4
except TypeError:
    print("TypeError")

# can't convert list to float
try:
    float([])
except TypeError:
    print("TypeError")

# test constant float with more than 255 chars
x = 1.84728699436059052516398251149631771898472869943605905251639825114963177189847286994360590525163982511496317718984728699436059052516398251149631771898472869943605905251639825114963177189847286994360590525163982511496317718984728699436059052516398251149631771898472869943605905251639825114963177189
print("%.5f" % x)
