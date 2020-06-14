# Test a class that behaves correctly

class Test1:
    def __float__(self):
        return 123.4
    def __complex__(self):
        return 123.4j

print(float(Test1()))
print(complex(Test1()))

# Test a class that returns the wrong type

class Test2:
    def __float__(self):
        return "Not a float"
    def __complex__(self):
        return "Not very complex"

try:
    print(float(Test2()))
except TypeError:
    print("TypeError")

try:
    print(complex(Test2()))
except TypeError:
    print("TypeError")

# Test a class that doesn't implement the conversion methods

class Test3:
    pass

try:
    print(float(Test3()))
except TypeError:
    print("TypeError")

try:
    print(complex(Test3()))
except TypeError:
    print("TypeError")
