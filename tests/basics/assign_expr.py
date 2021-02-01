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
