# test equality

print(None == None)

print(False == None)
print(False == False)
print(False == True)

print(() == ())
print(() == [])
print([] == [])
print(() == {})
print({} == ())

print(() == None)
print(() == False)
print(() == print)

print([] == None)
print([] == False)
print([] == print)

print({} == None)
print({} == False)
print({} == print)

print(1 == 1)
print(1 == 2)
print(1 == ())
print(1 == [])
print(1 == {})
print(1 == 'a')

print('a' == 'a')
print('a' == 'ab')
print('a' == 1)
print('a' == ())

# same as above but with !=

print(None != None)

print(False != None)
print(False != False)
print(False != True)

print(() != ())
print(() != [])
print([] != [])
print(() != {})
print({} != ())

print(() != None)
print(() != False)
print(() != print)

print([] != None)
print([] != False)
print([] != print)

print({} != None)
print({} != False)
print({} != print)

print(1 != 1)
print(1 != 2)
print(1 != ())
print(1 != [])
print(1 != {})
print(1 != 'a')

print('a' != 'a')
print('a' != 'ab')
print('a' != 1)
print('a' != ())
