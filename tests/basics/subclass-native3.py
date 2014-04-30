class MyExc(Exception):
    pass

e = MyExc(100, "Some error")
print(e)
# TODO: Prints native base class name
#print(repr(e))
print(e.args)
