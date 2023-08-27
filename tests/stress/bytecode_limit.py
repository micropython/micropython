# Test the limits of bytecode generation.

body = " with f()()() as a:\n  try:\n   f()()()\n  except Exception:\n   pass\n"

# Test overflow of jump offset.
for n in (430, 431, 432, 433):
    try:
        exec("cond = 0\nif cond:\n" + body * n + "else:\n print('cond false')\n")
    except MemoryError:
        print("SKIP")
        raise SystemExit
    except RuntimeError:
        print("RuntimeError")

# Test changing size of code info (source line/bytecode mapping) due to changing
# bytecode size in the final passes.  This test is very specific to how the
# code info is encoded, and how jump offsets shrink in the final passes.  This
# test should fail if the bytecode emitter doesn't correctly handle shrinking of
# the code info section.
exec(
    """
x = 0
if x:
"""
    + body * 13
    + """
x = [1 if x else 123]






print(x)
"""
)
