# del name

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
    # FIXME MicroPython returns KeyError for this
    print("NameError")

class C:
    def f():
        pass
