import skip_if
skip_if.no_cpython_compat()

try:
    from collections import namedtuple
except ImportError:
    skip_if.skip()

# Try single string
T3 = namedtuple("TupComma", "foo bar")
t = T3(1, 2)
print(t.foo, t.bar)

# Try single string with comma field separator
# Not implemented so far
#T2 = namedtuple("TupComma", "foo,bar")
#t = T2(1, 2)
