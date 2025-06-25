# Test t-strings without "string_template" prefix
t = t"Hello World"
print(t.strings)
print(len(t.interpolations))