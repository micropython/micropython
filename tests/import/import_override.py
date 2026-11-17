# test overriding __import__ combined with importing from the filesystem


def custom_import(name, globals, locals, fromlist, level):
    # CPython always tries to import _io, so just let that through as-is.
    if name == "_io":
        return orig_import(name, globals, locals, fromlist, level)

    print("import", name, fromlist, level)

    class M:
        var = 456

    return M


orig_import = __import__
try:
    __import__("builtins").__import__ = custom_import
except AttributeError:
    print("SKIP")
    raise SystemExit

# import1a will be done via normal import which will import1b via our custom import
orig_import("import1a")
