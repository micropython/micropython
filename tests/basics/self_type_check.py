# make sure type of first arg (self) to a builtin method is checked
import skip_if
skip_if.board_in("gemma_m0", "trinket_m0")

list.append

try:
    list.append()
except TypeError as e:
    print("TypeError")

try:
    list.append(1)
except TypeError as e:
    print("TypeError")

try:
    list.append(1, 2)
except TypeError as e:
    print("TypeError")

l = []
list.append(l, 2)
print(l)

try:
    getattr(list, "append")(1, 2)
except TypeError as e:
    print("TypeError")

l = []
getattr(list, "append")(l, 2)
print(l)
