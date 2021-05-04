# test builtin exec with a buffer (bytearray/memoryview) input

try:
    exec
    bytearray
    memoryview
except:
    print("SKIP")
    raise SystemExit

exec(bytearray(b'print(1)'))
exec(memoryview(b'print(2)'))
