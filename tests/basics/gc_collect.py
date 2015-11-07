try:
    import gc
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

stepsize = 100
raymax = 100 * stepsize
iterlength = 10

# thrash the memory system
for start in range(stepsize, iterlength * stepsize, stepsize):
    numbers = list(range(start, start + raymax, stepsize))
    expected = ['value={}'.format(n) for n in numbers]
    values = ['something={}'.format(n) for n in numbers]
    result = [n.replace('something=', 'value=') for n in values]
    assert result == expected
    result = [int(n[n.index('=') + 1:]) for n in result]
    expected = list(range(start, start + raymax, stepsize))
    assert result == expected
    gc.collect()
