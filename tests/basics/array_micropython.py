# test MicroPython-specific features of array.array
try:
    import uarray as array
except ImportError:
    try:
        import array
    except ImportError:
        print("SKIP")
        raise SystemExit

# arrays of objects
a = array.array('O')
a.append(1)
print(a[0])

# arrays of pointers
a = array.array('P')
a.append(1)
print(a[0])
