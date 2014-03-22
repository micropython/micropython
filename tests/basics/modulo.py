# check modulo matches python definition
# This test compiler version
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

if False:
	print(1.23456 % 0.7)
	print(-1.23456 % 0.7)
	print(1.23456 % -0.7)
	print(-1.23456 % -0.7)

	a = 1.23456
	b = 0.7
	print(a % b)
	print(a % -b)
	print(-a % b)
	print(-a % -b)

