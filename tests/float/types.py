# float types

print(float)
print(complex)

print(type(float()) == float)
print(type(complex()) == complex)

print(type(0.0) == float)
print(type(1j) == complex)

# hashing float types

d = dict()
d[float] = complex
d[complex] = float
print(len(d))
