# calling a function from a function

def f(x):
    print(x + 1)

def g(x):
    f(2 * x)
    f(4 * x)

g(3)
a = 3
g((a)) # Making sure the extra parentheses cause no issues
