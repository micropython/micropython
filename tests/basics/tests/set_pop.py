s = {1}
print(s.pop())
try:
    print(s.pop(), "!!!")
except KeyError:
    pass
else:
    print("Failed to raise KeyError")

