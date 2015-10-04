def f1(**kwargs):
    print(kwargs)

f1()
f1(a=1)

def f2(a, **kwargs):
    print(a, kwargs)

f2(1)
f2(1, b=2)

def f3(a, *vargs, **kwargs):
    print(a, vargs, kwargs)

f3(1)
f3(1, 2)
f3(1, b=2)
f3(1, 2, b=3)

def f4(*vargs, **kwargs):
    print(vargs, kwargs)
f4(*(1, 2))
f4(kw_arg=3)
f4(*(1, 2), kw_arg=3)
