# check modulo matches python definition

# This tests compiler version
print(123 // 7)
print(-123 // 7)
print(123 // -7)
print(-123 // -7)

a = 10000001
b = 10000000
print(a // b)
print(a // -b)
print(-a // b)
print(-a // -b)

if True:
    a = 987654321987987987987987987987
    b = 19

    print(a // b)
    print(a // -b)
    print(-a // b)
    print(-a // -b)
    a = 10000000000000000000000000000000000000000000
    b = 100
    print(a // b)
    print(a // -b)
    print(-a // b)
    print(-a // -b)    
