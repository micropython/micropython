a = '¢пр'

print(a[0], a[0:1])
print(a[1], a[1:2])
print(a[2], a[2:3])
try:
    print(a[3])
except IndexError:
    print("IndexError")
print(a[3:4])

print(a[-1])
print(a[-2], a[-2:-1])
print(a[-3], a[-3:-2])
try:
    print(a[-4])
except IndexError:
    print("IndexError")
print(a[-4:-3])

print(a[0:2])
print(a[1:3])
print(a[2:4])
