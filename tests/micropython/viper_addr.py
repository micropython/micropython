# test passing addresses to viper


@micropython.viper
def get_addr(x: ptr) -> ptr:
    return x


@micropython.viper
def memset(dest: ptr8, c: int, n: int):
    for i in range(n):
        dest[i] = c


@micropython.viper
def memsum(src: ptr8, n: int) -> int:
    s = 0
    for i in range(n):
        s += src[i]
    return s


# create array and get its address
ar = bytearray(b"0000")
addr = get_addr(ar)
print(type(ar))
print(type(addr))
print(ar)

# pass array as an object
memset(ar, ord("1"), len(ar))
print(ar)

# pass direct pointer to array buffer
memset(addr, ord("2"), len(ar))
print(ar)

# pass direct pointer to array buffer, with offset
memset(addr + 2, ord("3"), len(ar) - 2)
print(ar)

# pass a read-only bytes object in
print(memsum(b"\x01\x02\x03\x04", 4))
