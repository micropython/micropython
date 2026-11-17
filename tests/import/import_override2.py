# test overriding __import__ combined with importing from the filesystem


def custom_import(name, globals, locals, fromlist, level):
    if level > 0:
        print("import", name, fromlist, level)
    return orig_import(name, globals, locals, fromlist, level)


orig_import = __import__
try:
    __import__("builtins").__import__ = custom_import
except AttributeError:
    print("SKIP")
    raise SystemExit

# import calls __import__ behind the scenes
import pkg7.subpkg1.subpkg2.mod3
