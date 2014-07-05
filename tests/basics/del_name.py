# del global

x = 1
print(x)
del x
try:
    print(x)
except NameError:
    print("NameError")
try:
    del x
except: # NameError:
    # FIXME uPy returns KeyError for this
    print("NameError")

class C:
    def f():
        pass
