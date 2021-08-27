# ¥ is 1 byte wide
# Œ is 2 bytes wide
# 😅 is 4 bytes wide

a = "hello¥Œ😅.txt\n\r\t'\"\\"

print(a)
print(repr(a))
