# Test slicing of Unicode strings

try:
    s = "Привет"
except:
    # In case Unicode is disabled
    print("SKIP")

print(s[:])
print(s[2:])
print(s[:5])
print(s[2:5])
print(s[2:5:1])
print(s[2:10])
print(s[-3:10])
print(s[-4:10])
