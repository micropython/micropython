# test %c formatting with unicode characters (issue #3364)
# tests that character codes >= 128 are properly encoded as UTF-8

# ASCII character
print("%c" % 65)

# 2-byte UTF-8 characters
print("%c" % 128)
print("%c" % 169)  # copyright symbol ©
print("%c" % 255)

# 3-byte UTF-8 character
print("%c" % 0x4E00)  # CJK ideograph 一

# 4-byte UTF-8 character
print("%c" % 0x1F600)  # emoji 😀

# test with .format() method
print("{:c}".format(169))
print("{:c}".format(0x4E00))

# test with f-strings
c = 169
print(f"{c:c}")
c = 0x1F600
print(f"{c:c}")

# test formatting with width
print("[%5c]" % 169)
print("[{:5c}]".format(0x4E00))
