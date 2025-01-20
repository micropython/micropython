class mylist(list):
    pass

a = mylist([1, 2, 5])
# Test setting instance attr
a.attr = "something"
# Test base type __str__
print(a)
# Test getting instance attr
print(a.attr)
# Test base type ->subscr
print(a[-1])
a[0] = -1
print(a)
# Test another base type unary op
print(len(a))

# Test binary op of base type, with 2nd arg being raw base type
print(a + [20, 30, 40])
# Test binary op of base type, with 2nd arg being same class as 1st arg
# TODO: Faults
#print(a + a)

# subclassing a type that doesn't have make_new at the C level (not allowed)
try:
    class myfunc(type([].append)):
        pass
except TypeError:
    print("TypeError")

# multiple bases with layout conflict
try:
    class A(type, tuple):
        None
except TypeError:
    print('TypeError')
