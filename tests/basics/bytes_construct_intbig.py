# test construction of bytes from different objects
# requires: int-big

# long ints
print(ord(bytes([14953042807679334000 & 0xff])))
