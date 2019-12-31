class A:
    def __eq__(self, other):
        print("A __eq__ called")
        return True

class B:
    def __ne__(self, other):
        print("B __ne__ called")
        return True

class C:
    def __eq__(self, other):
        print("C __eq__ called")
        return False

class D:
    def __ne__(self, other):
        print("D __ne__ called")
        return False

a = A()
b = B()
c = C()
d = D()

def test(s):
    print(s)
    print(eval(s))

for x in 'abcd':
    for y in 'abcd':
        test('{} == {}'.format(x,y))
        test('{} != {}'.format(x,y))
