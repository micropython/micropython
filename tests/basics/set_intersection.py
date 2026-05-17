s = {1, 2, 3, 4}
print(sorted(s))
print(sorted(s.intersection({1, 3})))
print(sorted(s.intersection([3, 4])))

print(s.intersection_update([1]))
print(sorted(s))
