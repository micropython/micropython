class Base1:
    def __init__(self, *args):
        print("Base1.__init__", args)

class Clist1(Base1, list):
    pass

a = Clist1()
print(len(a))
# Not compliant - list assignment should happen in list.__init__, which is not called
# because there's Base1.__init__, but we assign in list.__new__
#a = Clist1([1, 2, 3])
#print(len(a))

print("---")

class Clist2(list, Base1):
    pass

# Not compliant - should call list.__init__, but we don't have it
#a = Clist2()
#print(len(a))

# Not compliant - should call list.__init__, but we don't have it
#a = Clist2([1, 2, 3])
#print(len(a))
