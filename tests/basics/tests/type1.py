# basic types

print(type(type(None)))                # <class 'type'>
print(type(None))                      # <class 'NoneType'>
print(type(1))                         # <class 'int'>
print(type('a'))                       # <class 'str'>
print(type(1.23))                      # <class 'float'>
print(type([1,2,3]))                   # <class 'list'>
print(type({'a','b'}))                 # <class 'set'>
print(type(...))                       # <class 'ellipsis'>
print(type(True))                      # <class 'bool'>
print(type(NotImplemented))            # <class 'NotImplementedType'>
print(type({'Micro': 1, 'Python': 0})) # <class 'dict'>
