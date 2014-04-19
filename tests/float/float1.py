# basic float
x = 1 / 2
print(x)

print(1.0 // 2)
print(2.0 // 2)

try:
    1.0 / 0
except ZeroDivisionError:
    print("ZeroDivisionError")

try:
    1.0 // 0
except ZeroDivisionError:
    print("ZeroDivisionError")
