try:
    f()
except A:
    g()
except:
    h()

try:
    f()
except A:
    g()
except B as c:
    h()

try:
    f()
except A:
    g()
except B as c:
    h()
except:
    i()
