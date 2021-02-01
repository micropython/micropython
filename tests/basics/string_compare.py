print("" == "")
print("" > "")
print("" < "")
print("" == "1")
print("1" == "")
print("" > "1")
print("1" > "")
print("" < "1")
print("1" < "")
print("" >= "1")
print("1" >= "")
print("" <= "1")
print("1" <= "")

print("1" == "1")
print("1" != "1")
print("1" == "2")
print("1" == "10")

print("1" > "1")
print("1" > "2")
print("2" > "1")
print("10" > "1")
print("1/" > "1")
print("1" > "10")
print("1" > "1/")

print("1" < "1")
print("2" < "1")
print("1" < "2")
print("1" < "10")
print("1" < "1/")
print("10" < "1")
print("1/" < "1")

print("1" >= "1")
print("1" >= "2")
print("2" >= "1")
print("10" >= "1")
print("1/" >= "1")
print("1" >= "10")
print("1" >= "1/")

print("1" <= "1")
print("2" <= "1")
print("1" <= "2")
print("1" <= "10")
print("1" <= "1/")
print("10" <= "1")
print("1/" <= "1")

# this tests an internal string that doesn't have a hash with a string
# that does have a hash, but the lengths of the two strings are different
try:
    import usys as sys
except ImportError:
    import sys
print(sys.version == 'a long string that has a hash')

# this special string would have a hash of 0 but is incremented to 1
print('Q+?' == 'Q' + '+?')
