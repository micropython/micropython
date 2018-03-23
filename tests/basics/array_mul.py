try:
    import array
except ImportError:
    print("SKIP")
    raise SystemExit

a1 = array.array('I', [1])
a2 = array.array('I', [2]) * 2
a3 = (a1 + a2)
print(a3)

a3 *= 5
print(a3)

a3 *= 0
print(a3)

a4 = a2 * 0
print(a4)

a4 *= 0
print(a4)

a4 = a4 * 2
print(a4)

a4 *= 2
print(a4)
