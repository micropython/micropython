# del global

def do_del():
    global x
    del x

x = 1
print(x)
do_del()
try:
    print(x)
except NameError:
    print("NameError")
try:
    do_del()
except: # NameError:
    # FIXME uPy returns KeyError for this
    print("NameError")

# delete globals using a list

a = 1
del (a,)
try:
    print(a)
except NameError:
    print("NameError")

a = 2
b = 3
del (a, b)
try:
    print(a)
except NameError:
    print("NameError")
try:
    print(b)
except NameError:
    print("NameError")

a = 1
b = 2
c = 3
del (a, b, c)
try:
    print(a)
except NameError:
    print("NameError")
try:
    print(b)
except NameError:
    print("NameError")
try:
    print(c)
except NameError:
    print("NameError")
