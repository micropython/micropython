# cmdline: -X compile-only
# test compile-only functionality
print("This should not be printed")
x = 1 + 2


def hello():
    return "world"


class TestClass:
    def __init__(self):
        self.value = 42
