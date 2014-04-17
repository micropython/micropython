# basic types

print(bool)
print(int)
print(tuple)
print(list)
print(set)
print(dict)

print(type(bool()) == bool)
print(type(int()) == int)
print(type(tuple()) == tuple)
print(type(list()) == list)
print(type(set()) == set)
print(type(dict()) == dict)

print(type(False) == bool)
print(type(0) == int)
print(type(()) == tuple)
print(type([]) == list)
print(type({None}) == set)
print(type({}) == dict)

try:
    bool.foo
except AttributeError:
    print("AttributeError")
