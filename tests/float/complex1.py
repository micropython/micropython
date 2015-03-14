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
print(1j + 2)
print(1j + 2j)
print(1j - 2)
print(1j - 2j)
print(1j * 2)
print(1j * 2j)
print(1j / 2)
print(1j / (1 + 2j))
ans = 1j ** 2.5; print("%.5g %.5g" % (ans.real, ans.imag))
ans = 1j ** 2.5j; print("%.5g %.5g" % (ans.real, ans.imag))

# builtin abs
print(abs(1j))
print("%.5g" % abs(1j + 2))
