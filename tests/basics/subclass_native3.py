class MyExc(Exception):
    pass

e = MyExc(100, "Some error")
print(e)
print(repr(e))
print(e.args)

try:
    raise MyExc("Some error")
except MyExc as e:
    print("Caught exception:", repr(e))

try:
    raise MyExc("Some error2")
except Exception as e:
    print("Caught exception:", repr(e))

try:
    raise MyExc("Some error2")
except:
    print("Caught user exception")
