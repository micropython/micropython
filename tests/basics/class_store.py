# store to class vs instance

class C:
    pass

c = C()
c.x = 1
print(c.x)
C.x = 2
C.y = 3
print(c.x, c.y)
print(C.x, C.y)
print(C().x, C().y)
c = C()
print(c.x)
c.x = 4
print(c.x)
