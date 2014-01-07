d={1:2,3:4}
print(d.popitem())
print(d)
print(d.popitem())
print(d)
try:
    print(d.popitem(), "!!!",)
except KeyError:
    print("Raised KeyError")
else:
    print("Did not raise KeyError")
