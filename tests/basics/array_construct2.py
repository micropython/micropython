try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# construct from something with unknown length (requires generators)
print(array('i', (i for i in range(10))))
