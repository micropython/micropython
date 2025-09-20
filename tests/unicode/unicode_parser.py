# test invalid UTF-8 string via eval
try:
    eval(b"'ab\xa1'")
except SyntaxError:
    print("Exception")
try:
    eval(b"'ab\xf8'")
except SyntaxError:
    print("Exception")
try:
    eval(bytearray(b"'ab\xc0a'"))
except SyntaxError:
    print("Exception")
try:
    eval(b"'\xf0\xe0\xed\xe8'")
except SyntaxError:
    print("Exception")

try:
    exec(b"b\xff = 1")
except SyntaxError:
    print("Exception")
