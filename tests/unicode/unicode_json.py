# Test json.loads with \u escapes outside the BMP.

try:
    import json
except ImportError:
    print("SKIP")
    raise SystemExit

# A surrogate pair decodes to one character.
s = json.loads('"\\ud83d\\ude00"')
print(len(s), hex(ord(s)))
print(json.loads('"a\\uD834\\uDD1Eb"'))

# A lone surrogate stays as is.
print(len(json.loads('"\\ud83d"')), len(json.loads('"\\ude00\\ud83d"')))

# Only a high surrogate escape combines with the low surrogate escape after it.
print(len(json.loads(b'"\xed\xa0\xbd\\ude00"')))
