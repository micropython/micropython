# check modulo matches python definition

# this tests compiler constant folding
print(123 % 7)
print(-123 % 7)
print(123 % -7)
print(-123 % -7)

a = 321
b = 19
print(a % b)
print(a % -b)
print(-a % b)
print(-a % -b)


a = 987654321987987987987987987987
b = 19

print(a % b)
print(a % -b)
print(-a % b)
print(-a % -b)
