# test passing addresses to viper

@micropython.viper
def get_addr(x:ptr) -> ptr:
    return x

@micropython.viper
def memset(dest:ptr8, c:int, n:int):
    for i in range(n):
        dest[i] = c

# create array and get its address
ar = bytearray('0000')
addr = get_addr(ar)
print(type(ar))
print(type(addr))
print(ar)

# pass array as an object
memset(ar, ord('1'), len(ar))
print(ar)

# pass direct pointer to array buffer
memset(addr, ord('2'), len(ar))
print(ar)

# pass direct pointer to array buffer, with offset
memset(addr + 2, ord('3'), len(ar) - 2)
print(ar)
