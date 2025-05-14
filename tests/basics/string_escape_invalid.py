# Test invalid escape characters.
# CPython issues a SyntaxWarning for this.

print("\z")
