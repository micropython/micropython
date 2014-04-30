class Base1:
    def __init__(self, *args):
        print("Base1.__init__",args)

class Clist1(Base1, list):
    pass

class Ctuple1(Base1, tuple):
    pass

a = Clist1()
print(len(a))
a = Clist1([1, 2, 3])
print(len(a))

a = Ctuple1()
print(len(a))
a = Ctuple1([1, 2, 3])
# TODO: Faults
#print(len(a))

print("---")

class Clist2(list, Base1):
    pass

class Ctuple2(tuple, Base1):
    pass
a = Clist2()
print(len(a))
a = Clist2([1, 2, 3])
print(len(a))

#a = Ctuple2()
#print(len(a))
#a = Ctuple2([1, 2, 3])
#print(len(a))
