class DivClass:
    def __truediv__(self, other):
        return "truediv"

    def __rtruediv__(self, other):
        return "rtruediv"

    def __floordiv__(self, other):
        return "floordiv"

    def __rfloordiv__(self, other):
        return "rfloordiv"


print(DivClass() / "")  # truediv
print("" / DivClass())  # rtruediv
print(DivClass() // "")  # floordiv
print("" // DivClass())  # rfloordiv

# Test inplace
val = DivClass()
val /= ""
print(val)  # truediv

val = ""
val /= DivClass()
print(val)  # rtruediv

val = DivClass()
val //= ""
print(val)  # floordiv

val = ""
val //= DivClass()
print(val)  # rfloordiv
