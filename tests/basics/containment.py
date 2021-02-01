# sets, see set_containment
for i in 1, 2:
    for o in {1:2}, {1:2}.keys():
        print("{} in {}: {}".format(i, str(o), i in o))
        print("{} not in {}: {}".format(i, str(o), i not in o))

haystack = "supercalifragilistc"
for needle in [haystack[i:] for i in range(len(haystack))]:
    print(needle, "in", haystack, "::", needle in haystack)
    print(needle, "not in", haystack, "::", needle not in haystack)
    print(haystack, "in", needle, "::", haystack in needle)
    print(haystack, "not in", needle, "::", haystack not in needle)
for needle in [haystack[:i+1] for i in range(len(haystack))]:
    print(needle, "in", haystack, "::", needle in haystack)
    print(needle, "not in", haystack, "::", needle not in haystack)
    print(haystack, "in", needle, "::", haystack in needle)
    print(haystack, "not in", needle, "::", haystack not in needle)

# containment of bytes/ints in bytes
print(b'' in b'123')
print(b'0' in b'123', b'1' in b'123')
print(48 in b'123', 49 in b'123')

# containment of int in str is an error
try:
    1 in '123'
except TypeError:
    print('TypeError')

# until here, the tests would work without the 'second attempt' iteration thing.

for i in 1, 2:
    for o in [], [1], [1, 2]:
        print("{} in {}: {}".format(i, o, i in o))
        print("{} not in {}: {}".format(i, o, i not in o))
