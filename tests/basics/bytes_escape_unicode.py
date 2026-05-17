# Coverage test for unicode escape in a bytes literal.
# CPython issues a SyntaxWarning for this.
print(b"\u1234")
