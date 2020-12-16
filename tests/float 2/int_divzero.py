try:
    1 / 0
except ZeroDivisionError:
    print("ZeroDivisionError")

try:
    0 ** -1
except ZeroDivisionError:
    print("ZeroDivisionError")
