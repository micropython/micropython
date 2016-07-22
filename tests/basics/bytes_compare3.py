# Based on MicroPython config option, comparison of str and bytes
# or vice versa may issue a runtime warning. On CPython, if run as
# "python3 -b", only comparison of str to bytes issues a warning,
# not the other way around (while exactly comparison of bytes to
# str would be the most common error, as in sock.recv(3) == "GET").
# Update: the issue above with CPython apparently happens in REPL,
# when run as a script, both lines issue a warning.
print("123" == b"123")
print(b"123" == "123")
