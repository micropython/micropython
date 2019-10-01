print("Yep, I got imported.")

try:
    x = const(1)
except NameError:
    print('const not defined')

const = lambda x: x

_CNT01 = "CONST01"
_CNT02 = const(123)
A123 = const(123)
a123 = const(123)

def dummy():
    return False

def saysomething():
    print("There, I said it.")

def neverexecuted():
    print("Never got here!")

print("Yep, got here")
