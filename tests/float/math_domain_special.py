# Tests domain errors in special math functions

try:
    import math
    math.erf
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

inf = float('inf')
nan = float('nan')

# single argument functions
for name, f, args in (
    ('expm1', math.exp, ()),
    ('log2', math.log2, (-1, 0)),
    ('log10', math.log10, (-1, 0)),
    ('sinh', math.sinh, ()),
    ('cosh', math.cosh, ()),
    ('tanh', math.tanh, ()),
    ('asinh', math.asinh, ()),
    ('acosh', math.acosh, (-1, 0.9, 1)),
    ('atanh', math.atanh, (-1, 1)),
    ('erf', math.erf, ()),
    ('erfc', math.erfc, ()),
    ('gamma', math.gamma, (-2, -1, 0, 1)),
    ('lgamma', math.lgamma, (-2, -1, 0, 1)),
    ):
    for x in args + (inf, -inf, nan):
        try:
            ans = f(x)
            print('%.4f' % ans)
        except ValueError:
            print(name, 'ValueError')
        except OverflowError:
            print(name, 'OverflowError')
