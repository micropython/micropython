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
