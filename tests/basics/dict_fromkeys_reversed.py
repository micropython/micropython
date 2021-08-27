# Skip this test if reversed() isn't built in.
import skip_if
skip_if.no_reversed()

# argument to fromkeys has no __len__
d = dict.fromkeys(reversed(range(1)))
print(d)
