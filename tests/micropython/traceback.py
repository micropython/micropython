# This comment allows test runners like run_script_on_remote_target
# to safely prepend a line of code without messing up line numbers.


def foo():
    1 / 0


def normalize(filename):
    if filename == "<stdin>":
        return "traceback.py"
    return filename.split("/")[-1]


try:
    foo()
except Exception as e:
    tb = e.__traceback__
    for t in tb:
        print("%s:%d" % (normalize(t[0]), t[1]), *t[2:])
    e.__traceback__ = None
    print(e.__traceback__)
    try:
        e.args = 77
    except AttributeError:
        print("setting args not allowed")
