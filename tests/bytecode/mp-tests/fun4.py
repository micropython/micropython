def f(a, b=1, *c, d):
    pass
    #print(a,b,c,d) # bug in uPy!
f = lambda a, b, *c, d: None # default arg
#f = lambda a, b=1, *c, d: None # default arg for lambda not implemented
