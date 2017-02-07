# test basic complex number functionality

# constructor
print(complex(1))
print(complex(1.2))
print(complex(1.2j))
print(complex("1"))
print(complex("1.2"))
print(complex("1.2j"))
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
ans = 0j ** 0; print("%.5g %.5g" % (ans.real, ans.imag))
ans = 0j ** 1; print("%.5g %.5g" % (ans.real, ans.imag))
ans = 0j ** 0j; print("%.5g %.5g" % (ans.real, ans.imag))
ans = 1j ** 2.5; print("%.5g %.5g" % (ans.real, ans.imag))
ans = 1j ** 2.5j; print("%.5g %.5g" % (ans.real, ans.imag))

# comparison
print(1j == 1)
print(1j == 1j)

# builtin abs
print(abs(1j))
print("%.5g" % abs(1j + 2))

# float on lhs should delegate to complex
print(1.2 + 3j)

# check printing of inf/nan
print(float('nan') * 1j)
print(float('inf') * (1 + 1j))
print(float('-inf') * (1 + 1j))

# convert bignum to complex on rhs
ans = 1j + (1 << 70); print("%.5g %.5g" % (ans.real, ans.imag))

# can't assign to attributes
try:
    (1j).imag = 0
except AttributeError:
    print('AttributeError')

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

#small int on LHS, complex on RHS, unsupported op
try:
    print(1 | 1j)
except TypeError:
    print('TypeError')

# zero division
try:
    1j / 0
except ZeroDivisionError:
    print("ZeroDivisionError")

# zero division via power
try:
    0j ** -1
except ZeroDivisionError:
    print("ZeroDivisionError")
try:
    0j ** 1j
except ZeroDivisionError:
    print("ZeroDivisionError")
