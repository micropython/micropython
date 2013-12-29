# scope

gl = 1

def f(x):
    global gl
    gl += 2
    lo1 = 3
    lo2 = 4
    lo3 = 5

    def f2(x, y):
        global gl
        nonlocal lo3
        lo3 = 5
        lo4 = gl + lo2 + lo3

    return f2
