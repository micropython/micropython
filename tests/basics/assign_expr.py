(x := 4)
print(x)

if x := 2:
    print(True)
print(x)

print(4, x := 5)
print(x)

x = 1
print(x, x := 5, x)
print(x)

# Test "while" with assignment expression as conditional, assigning to a new local.
# The while conditional is compiled after the while body, so this tests how the
# compiler handles the case of an unbound local being compiled before it is assigned.
def f():
    l = [0, 1]
    while local := len(l):
        print(local, l.pop())


f()


def foo():
    print("any", any((hit := i) % 5 == 3 and (hit % 2) == 0 for i in range(10)))
    return hit


hit = 123
print(foo())
print(hit)  # shouldn't be changed by foo

print("any", any((hit := i) % 5 == 3 and (hit % 2) == 0 for i in range(10)))
print(hit)  # should be changed by above

print([((m := k + 1), k * m) for k in range(4)])
print(m)
