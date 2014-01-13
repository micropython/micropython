s = {1}
print(s.remove(1))
print(list(s))
try:
    print(s.remove(1), "!!!")
except KeyError:
    pass
else:
    print("failed to raise KeyError")
