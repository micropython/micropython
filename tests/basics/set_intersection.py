s = {1, 2, 3, 4}
print(sorted(s))
print(sorted(s.intersection()))
print(sorted(s))
print(s.intersection_update())
print(sorted(s))
print(sorted(s.intersection({1, 3})))
print(sorted(s))
print(sorted(s.intersection([3, 4])))
print(sorted(s))
print(sorted(s.intersection({1, 2, 3}, {1, 4, 5}, {1})))
print(sorted(s))
print(sorted(s.intersection([1, 3], [1, 3, 5])))
print(sorted(s))

print(s.intersection_update([1]))
print(sorted(s))

s = {1, 2, 3}
print(s.intersection_update({1, 2}, {1}))
print(sorted(s))
