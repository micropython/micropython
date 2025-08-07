# test invalid UTF-8 string via eval
# Passing byte strings to exec/eval is a micropython extension
try:
    eval(b"'ab\xa1'")
except UnicodeError:
    print("UnicodeError")
try:
    eval(b"'ab\xf8'")
except UnicodeError:
    print("UnicodeError")
try:
    eval(bytearray(b"'ab\xc0a'"))
except UnicodeError:
    print("UnicodeError")
try:
    eval(b"'\xf0\xe0\xed\xe8'")
except UnicodeError:
    print("UnicodeError")

try:
    exec(b"b\xff = 1")
except UnicodeError:
    print("UnicodeError")
