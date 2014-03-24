# basic types

print(bool)
print(int)
print(float)
print(complex)
print(tuple)
print(list)
print(set)
print(dict)

print(type(bool()) == bool)
print(type(int()) == int)
print(type(float()) == float)
print(type(complex()) == complex)
print(type(tuple()) == tuple)
print(type(list()) == list)
print(type(set()) == set)
print(type(dict()) == dict)

print(type(False) == bool)
print(type(0) == int)
print(type(0.0) == float)
print(type(1j) == complex)
print(type(()) == tuple)
print(type([]) == list)
print(type({None}) == set)
print(type({}) == dict)

try:
    bool.foo
except AttributeError:
    print("AttributeError")
