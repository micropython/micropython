# Subclass from 2 bases explicitly subclasses from object

class Base1(object):
    pass

class Base2(object):
    pass

class Sub(Base1, Base2):
    pass

o = Sub()
