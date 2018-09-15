# test that viper functions capture their globals context

gl = {}

exec("""
@micropython.viper
def f():
    return x
""", gl)

# x is not yet in the globals, f should not see it
try:
    print(gl['f']())
except NameError:
    print('NameError')

# x is in globals, f should now see it
gl['x'] = 123
print(gl['f']())
