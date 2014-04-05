for i in range(100):
    d = dict()
    for j in range(100):
        d[j] = j
    del d[i]
    for j in range(100):
        if j not in d:
            print(j, 'not in d')
