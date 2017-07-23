def f():
    # list comprehension

    print([a + 1 for a in range(5)])
    print([(a, b) for a in range(3) for b in range(2)])
    print([a * 2 for a in range(7) if a > 3])

    print([a for a in [1, 3, 5]])
    print([a for a in [a for a in range(4)]])

    # dict comprehension

    d = {a : 2 * a for a in range(5)}
    print(d[0], d[1], d[2], d[3], d[4])

    # set comprehension
    # see set_comprehension.py

f()
