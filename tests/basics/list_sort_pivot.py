# test list.sort() and sorted() on input that is already in order, in reverse
# order, or otherwise awkward for the choice of pivot


# all permutations of l, made by swapping in place (no slices or itertools)
def permutations(l, k=0):
    if k >= len(l) - 1:
        yield list(l)
        return
    for i in range(k, len(l)):
        l[k], l[i] = l[i], l[k]
        for p in permutations(l, k + 1):
            yield p
        l[k], l[i] = l[i], l[k]


def is_sorted(l, reverse=False):
    for i in range(len(l) - 1):
        if (l[i + 1] < l[i]) if not reverse else (l[i] < l[i + 1]):
            return False
    return True


# every ordering of up to 6 elements, with and without duplicates, sorts
# correctly, with and without key and reverse
ok = True
for base in (
    [],
    [1],
    [1, 2],
    [1, 2, 3],
    [1, 1, 2],
    [1, 2, 3, 4],
    [2, 2, 1, 1],
    [1, 2, 3, 4, 5, 6],
    [3, 1, 3, 1, 3, 1],
):
    want = sorted(base)
    want_rev = sorted(base, reverse=True)
    for p in permutations(base):
        if sorted(p) != want:
            ok = False
        if sorted(p, reverse=True) != want_rev:
            ok = False
        if sorted(p, key=lambda x: -x) != want_rev:
            ok = False
        if sorted(p, key=lambda x: -x, reverse=True) != want:
            ok = False
print(ok)

# lists that are in order, in reverse, all equal, or "organ pipe" shaped (small
# enough for targets with a small heap)
n = 200
for l in (
    list(range(n)),
    list(range(n, 0, -1)),
    [7] * n,
    list(range(n // 2)) + list(range(n // 2, 0, -1)),
):
    a = sorted(l)
    print(a[0], a[-1], is_sorted(a))
    a = sorted(l, reverse=True)
    print(a[0], a[-1], is_sorted(a, True))
