# Test additional str.is*() methods
try:
    str.isalnum
except:
    print("SKIP")
    raise SystemExit

# isalnum
print("".isalnum())
print("abc123".isalnum())
print("abc".isalnum())
print("123".isalnum())
print("abc 123".isalnum())
print("abc!".isalnum())

# isascii
print("".isascii())
print("hello".isascii())
print("hello\x80".isascii())
print("\x7f".isascii())

# isdecimal
print("".isdecimal())
print("123".isdecimal())
print("abc".isdecimal())
print("12.3".isdecimal())

# isnumeric
print("".isnumeric())
print("123".isnumeric())
print("abc".isnumeric())

# isidentifier
print("".isidentifier())
print("hello".isidentifier())
print("_hello".isidentifier())
print("hello_world".isidentifier())
print("hello world".isidentifier())
print("123abc".isidentifier())
print("_".isidentifier())
print("a1".isidentifier())

# isprintable
print("".isprintable())
print("hello".isprintable())
print("hello world".isprintable())
print("hello\n".isprintable())
print("hello\t".isprintable())
print("\x00".isprintable())
