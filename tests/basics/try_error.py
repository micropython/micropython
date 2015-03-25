# test bad exception match

try:
    try:
        a
    except 1:
        pass
except TypeError:
    print("TypeError")

try:
    try:
        a
    except (1,):
        pass
except TypeError:
    print("TypeError")
