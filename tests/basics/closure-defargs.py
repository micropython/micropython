def f():
    a = 1
    def bar(b = 10, c = 20):
        print(a + b + c)
    bar()

print(f())

