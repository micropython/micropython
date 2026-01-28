# Test that super() handles native type subclasses without crashing.
# This tests the fix for a crash in mp_obj_class_lookup() when
# lookup->obj was accessed incorrectly.

# Test 1: Subclass of native type with super()
class MyList(list):
    def __init__(self, *args):
        super().__init__(*args)
        self.custom = "initialized"

ml = MyList([1, 2, 3])
print(len(ml))
print(ml.custom)

# Test 2: Exception subclass with super().__init__()
class MyError(ValueError):
    def __init__(self, msg, code):
        super().__init__(msg)
        self.code = code

try:
    raise MyError("error occurred", 42)
except MyError as e:
    print("caught:", e.args[0])
    print("code:", e.code)

# Test 3: Multiple levels of inheritance with native base
class MyList2(MyList):
    def __init__(self, *args):
        super().__init__(*args)
        self.custom2 = "also initialized"

ml2 = MyList2([4, 5, 6])
print(len(ml2))
print(ml2.custom)
print(ml2.custom2)

print("done")
