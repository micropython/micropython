# Test that objects stored via the 'O' typecode stay alive across collections.

try:
    import array, struct, gc

    array.array("O", [None])
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

a = array.array("O", [None])
for i in range(50):
    a[0] = [i, bytearray(5)]
    gc.collect()
    junk = [bytearray(16) for _ in range(50)]
    v = a[0]
    if v[0] != i or len(v) != 2:
        print("array", i, v)
        break
else:
    print("array ok")

b = struct.pack("O", [1, 2, 3, bytearray(40)])
for i in range(50):
    gc.collect()
    junk = [[i] * 4 for _ in range(60)]
v = struct.unpack("O", b)[0]
print(v[:3] == [1, 2, 3] and len(v) == 4)

buf = bytearray(struct.calcsize("O"))
struct.pack_into("O", buf, 0, [4, 5, 6])
for i in range(50):
    gc.collect()
    junk = [[i] * 4 for _ in range(60)]
print(struct.unpack_from("O", buf)[0] == [4, 5, 6])
