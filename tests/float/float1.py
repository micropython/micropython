# test basic float capabilities

# literals
print(0.12)
print(1.0)
print(1.2)
print(0e0)
print(0e0)
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
print(float("-nan"))
print(float("NaN"))
try:
    float("")
except ValueError:
    print("ValueError")
try:
    float("1e+")
except ValueError:
    print("ValueError")
try:
    float("1z")
except ValueError:
    print("ValueError")

# construct from something with the buffer protocol
print(float(b"1.2"))
print(float(bytearray(b"3.4")))

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
print(0.0 == False, 1.0 == True)
print(False == 0.0, True == 1.0)

# comparison of nan is special
nan = float("nan")
print(nan == 1.2)
print(nan == nan)

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

try:
    0.0 ** -1
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

# small int on LHS, float on RHS, unsupported op
try:
    print(1 | 1.0)
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
