class MyExc(Exception):
    pass

e = MyExc(100, "Some error")
print(e)
print(repr(e))
print(e.args)
