# test builtin locals()

x = 123
print(locals()['x'])

class A:
    y = 1
    def f(self):
        pass

    print('x' in locals())
    print(locals()['y'])
    print('f' in locals())
