# Test str.translate() and str.maketrans()
try:
    str.translate
except:
    print("SKIP")
    raise SystemExit

# maketrans and translate
table = str.maketrans("aeiou", "12345")
print("hello world".translate(table))

# translate with deletion (None values)
table = str.maketrans("aeiou", "12345", "ld")
print("hello world".translate(table))

# translate with empty string
print("".translate(table))

# maketrans basic
t = str.maketrans("abc", "xyz")
print("abcdef".translate(t))

# translate with manual dict
print("hello".translate({104: 72}))  # h -> H

# translate delete via None
print("hello".translate({108: None}))  # delete l

# translate no match - passthrough
print("abc".translate({}))

# translate with non-dict argument
try:
    "abc".translate(42)
except TypeError:
    print("TypeError")

# maketrans with mismatched lengths
try:
    str.maketrans("abc", "xy")
except ValueError:
    print("ValueError")
