print(exec("def foo(): return 42"))
print(foo())

d = {}
exec("def bar(): return 84", d)
print(d["bar"]())

# passing None/dict as args to globals/locals
foo = 11
exec('print(foo)')
exec('print(foo)', None)
exec('print(foo)', {'foo':3}, None)
exec('print(foo)', None, {'foo':3})
exec('print(foo)', None, {'bar':3})
exec('print(foo)', {'bar':3}, locals())

try:
    exec('print(foo)', {'bar':3}, None)
except NameError:
    print('NameError')

# invalid arg passed to globals
try:
    exec('print(1)', 'foo')
except TypeError:
    print('TypeError')

# invalid arg passed to locals
try:
    exec('print(1)', None, 123)
except TypeError:
    print('TypeError')
