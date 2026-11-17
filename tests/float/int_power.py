# negative power should produce float
# requires: float

x = 2
print(x**-2)

x = 3
x **= -2
print("%.5f" % x)
