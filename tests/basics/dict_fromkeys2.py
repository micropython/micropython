import skip_if
skip_if.no_reversed()

# argument to fromkeys has no __len__
d = dict.fromkeys(reversed(range(1)))
#d = dict.fromkeys((x for x in range(1)))
print(d)
