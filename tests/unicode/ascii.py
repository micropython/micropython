# Test conversions from ASCII to/from UTF8 strings via constructor

# Invalid ASCII characters via constructor
try:
    str(b"ni\xe5\xa5\xbd", "ascii")  # Valid utf8, invalid ascii
except UnicodeError:
    print("UnicodeError")

try:
    str(b"\xff\xee\xff", "ascii")  # Totally invalid
except UnicodeError:
    print("UnicodeError")


bytes("😀", "utf8")
bytes("abcd", "ascii")

# Conversion should fail if non-ASCII characters in string
try:
    bytes("😀", "ascii")
except UnicodeError:
    print("UnicodeError")
