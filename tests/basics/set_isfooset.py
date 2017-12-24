sets = [set(), {1}, {1, 2, 3}, {3, 4, 5}, {5, 6, 7}]
args = sets + [[1], [1, 2], [1, 2 ,3]]
for i in sets:
    for j in args:
        print(i.issubset(j))
        print(i.issuperset(j))
