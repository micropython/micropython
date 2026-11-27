# test that sorting a list that is already in order, or in reverse, does not
# take quadratic time (it does unless MICROPY_OPT_LIST_SORT_MEDIAN_PIVOT is
# enabled)


# an item that counts how often it is compared
class Counted:
    count = 0

    def __init__(self, x):
        self.x = x

    def __lt__(self, other):
        Counted.count += 1
        return self.x < other.x


def comparisons(n, reverse_input, reverse):
    items = [Counted(n - i if reverse_input else i) for i in range(n)]
    Counted.count = 0
    items.sort(reverse=reverse)
    return Counted.count


# skip if this build sorts presorted input in quadratic time: 64 items in order
# take about 2000 comparisons then, and a few hundred otherwise
if comparisons(64, False, False) > 1000:
    print("SKIP")
    raise SystemExit

# skip on targets without the memory for n items
n = 1000
try:
    items = [Counted(i) for i in range(n)]
    items = None
except MemoryError:
    print("SKIP")
    raise SystemExit

# n items in order or in reverse take far fewer comparisons than the
# n * n / 2 of a quadratic sort
for reverse_input in (False, True):
    for reverse in (False, True):
        print(reverse_input, reverse, comparisons(n, reverse_input, reverse) < n * 20)
