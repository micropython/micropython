# closing over a local variable in a list comprehension
def f():
    a = 1
    x = [a + b for b in l]
