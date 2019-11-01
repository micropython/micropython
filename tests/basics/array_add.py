# test array + array
try:
    import uarray as array
except ImportError:
    try:
        import array
    except ImportError:
        print("SKIP")
        raise SystemExit

a1 = array.array('I', [1])
a2 = array.array('I', [2])
print(a1 + a2)

a1 += array.array('I', [3, 4])
print(a1)

a1.extend(array.array('I', [5]))
print(a1)
