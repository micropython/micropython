# test %c formatting with unicode characters in f-strings

c = 169
print(f"{c:c}")
c = 0x1F600
print(f"{c:c}")
