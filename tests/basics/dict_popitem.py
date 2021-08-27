els = []
d = {1:2,3:4}
a = d.popitem()
print(len(d))
els.append(a)
a = d.popitem()
print(len(d))
els.append(a)
try:
    print(d.popitem(), "!!!",)
except KeyError:
    print("Raised KeyError")
else:
    print("Did not raise KeyError")
print(sorted(els))
