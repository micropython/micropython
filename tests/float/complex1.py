# test basic complex number functionality

# constructor
print(complex(1))
print(complex(1.2))
print(complex(1.2j))
print(complex("j"))
print(complex("J"))
print(complex("1"))
print(complex("1.2"))
print(complex("1.2j"))
print(complex("1+j"))
print(complex("1+2j"))
print(complex("-1-2j"))
print(complex("-1+2j"))
print(complex("+1-2j"))
print(complex(" -1-2j "))
print(complex(" +1-2j "))
print(complex(" -1+2j "))
print(complex("nanj"))
print(complex("nan-infj"))
print(complex(1, 2))
print(complex(1j, 2j))

# unary ops
print(bool(1j))
print(+(1j))
print(-(1 + 2j))

# binary ops
print(1j + False)
print(1j + True)
print(1j + 2)
print(1j + 2j)
print(1j - 2)
print(1j - 2j)
print(1j * 2)
print(1j * 2j)
print(1j / 2)
print((1j / 2j).real)
print(1j / (1 + 2j))
ans = 0j**0
print("%.5g %.5g" % (ans.real, ans.imag))
ans = 0j**1
print("%.5g %.5g" % (ans.real, ans.imag))
ans = 0j**0j
print("%.5g %.5g" % (ans.real, ans.imag))
ans = 1j**2.5
print("%.5g %.5g" % (ans.real, ans.imag))
ans = 1j**2.5j
print("%.5g %.5g" % (ans.real, ans.imag))

# comparison
print(1j == 1)
print(1j == 1j)
print(0 + 0j == False, 1 + 0j == True)
print(False == 0 + 0j, True == 1 + 0j)

# comparison of nan is special
nan = float("nan") * 1j
print(nan == 1j)
print(nan == nan)

# builtin abs
print(abs(1j))
print("%.5g" % abs(1j + 2))

# builtin hash
print(hash(1 + 0j))
print(type(hash(1j)))

# float on lhs should delegate to complex
print(1.2 + 3j)

# negative base and fractional power should create a complex
ans = (-1) ** 2.3
print("%.5g %.5g" % (ans.real, ans.imag))
ans = (-1.2) ** -3.4
print("%.5g %.5g" % (ans.real, ans.imag))

# check printing of inf/nan
print(float("nan") * 1j)
print(float("-nan") * 1j)
print(float("inf") * (1 + 1j))
print(float("-inf") * (1 + 1j))

# malformed complex strings
for test in ("1+2", "1j+2", "1+2j+3", "1+2+3j", "1 + 2j"):
    try:
        complex(test)
    except ValueError:
        print("ValueError", test)

# can't assign to attributes
try:
    (1j).imag = 0
except AttributeError:
    print("AttributeError")

# can't convert rhs to complex
try:
    1j + []
except TypeError:
    print("TypeError")

# unsupported unary op
try:
    ~(1j)
except TypeError:
    print("TypeError")

# unsupported binary op
try:
    1j // 2
except TypeError:
    print("TypeError")

# unsupported binary op
try:
    1j < 2j
except TypeError:
    print("TypeError")

# small int on LHS, complex on RHS, unsupported op
try:
    print(1 | 1j)
except TypeError:
    print("TypeError")

# zero division
try:
    1j / 0
except ZeroDivisionError:
    print("ZeroDivisionError")

# zero division via power
try:
    0j**-1
except ZeroDivisionError:
    print("ZeroDivisionError")
try:
    0j**1j
except ZeroDivisionError:
    print("ZeroDivisionError")
