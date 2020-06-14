class Test1:
    def __float__(self):
        return 123.4
    def __complex__(self):
        return 123.4j

class Test2:
    def _float__(self):
        return "Not a float"
    def __complex__(self):
        return "Not very complex"

print(float(Test1()))
print(complex(Test1()))

try:
    print(float(Test2()))
except TypeError:
    print("TypeError")

try:
    print(complex(Test2()))
except TypeError:
    print("TypeError")

