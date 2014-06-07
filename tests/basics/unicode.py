# Test a UTF-8 encoded literal
s = "asdf©qwer"
for i in range(len(s)):
    print("s[%d]: %s   %X"%(i, s[i], ord(s[i])))
# Test all three forms of Unicode escape, and
# all blocks of UTF-8 byte patterns
s = "a\xA9\xFF\u0123\u0800\uFFEE\U0001F44C"
for i in range(len(s)):
    print("s[%d]: %s   %X"%(i, s[i], ord(s[i])))
