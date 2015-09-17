
def f(*args, **kwargs):
    print(args, kwargs)

def print_ret(x):
    print(x)
    return x

f(*print_ret(['a', 'b']), kw_arg=print_ret(None))
