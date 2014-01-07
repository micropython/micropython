d = {1: 2, 3: 4}
print(d.pop(3), d)
print(d)
print(d.pop(1, 42), d)
print(d.pop(1, 42), d)
print(d.pop(1, None), d)
try:
    print(d.pop(1), "!!!",)
except KeyError:
    print("Raised KeyError")
else:
    print("Did not rise KeyError!")
