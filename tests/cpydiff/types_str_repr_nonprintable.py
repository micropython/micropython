"""
categories: Types,str
description: repr() may print some non-printable Unicode characters literally instead of as escape sequences
cause: MicroPython uses a simplified heuristic to determine printable characters, avoiding the need for a full Unicode character database (saves memory). It prints characters >= U+0080 (excluding surrogates) as UTF-8. CPython uses the Unicode database to identify non-printable characters like noncharacters (U+FFFx in each plane).
workaround: Accept the difference for embedded use cases, or use ascii() or manual escaping if exact control is needed.
"""

# These are noncharacters that CPython escapes but MicroPython prints
# showing as hex to avoid display issues in documentation tables
print("U+FFFF:", repr("\uffff").encode('utf-8').hex())
print("U+1FFFF:", repr("\U0001ffff").encode('utf-8').hex())
