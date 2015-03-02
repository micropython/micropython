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
#print(-(1j)) uPy doesn't print correctly

# binary ops
print(1j + 2)
print(1j + 2j)
print(1j - 2)
print(1j - 2j)
print(1j * 2)
print(1j * 2j)
print(1j / 2)
#print(1j / 2j) uPy doesn't print correctly
#print(1j ** 2) uPy doesn't print correctly
#print(1j ** 2j) uPy doesn't print correctly

# builtin abs
print(abs(1j))
print(abs(1j + 2))
