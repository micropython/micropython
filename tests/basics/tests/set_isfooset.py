sets = [set(), {1}, {1, 2, 3}, {3, 4, 5}, {5, 6, 7}]
for i in sets:
    for j in sets:
        print(i.issubset(j))
        print(i.issuperset(j))
