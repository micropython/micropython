# test array + array
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

a1.extend([6, 7])
print(a1)

a1.extend(i for i in (8, 9))
print(a1)
