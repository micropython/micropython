# test default args with lambda

f = lambda x=1: x
print(f(), f(2), f(x=3))

y = 'y'
f = lambda x=y: x
print(f())

f = lambda x, y=[]: (x, y)
f(0)[1].append(1)
print(f(1), f(x=2), f(3, 4), f(4, y=5))
