# generators and yield

def main():
    def f():
        print(123)
        yield
        print(456)
        yield 2
        print(789)

    a = f()
    print(a)
    print(a.__next__())
    print(a.__next__())
    #print(a.__next__())

main()
