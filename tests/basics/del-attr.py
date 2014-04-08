class C:
    def f():
        pass

# del a class attribute

del C.f
try:
    print(C.x)
except AttributeError:
    print("AttributeError")
try:
    del C.f
except AttributeError:
    print("AttributeError")

# del an instance attribute

c = C()

c.x = 1
print(c.x)

del c.x
try:
    print(c.x)
except AttributeError:
    print("AttributeError")
try:
    del c.x
except AttributeError:
    print("AttributeError")
