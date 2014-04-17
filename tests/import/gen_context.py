import gen_context2

GLOBAL = "GLOBAL"

def gen():
    print(GLOBAL)
    yield 1

gen_context2.call(gen())
