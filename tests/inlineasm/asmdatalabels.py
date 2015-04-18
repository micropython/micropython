import array

@micropython.asm_thumb
def f(r0):
    b(START)
    data(4, 0, 0)
    label(MYDATA)
    data(1, 111, 2) # TODO make an odd number of elements when data() is fixed
    label(START)
    getlbl(r1, r2, MYDATA)
    add(r3, r1, r2)
    ldrb(r4, [r3, 0])
    str(r4, [r0, 0])
    getlbl(r1, r2, FORWARDREF)
    add(r3, r1, r2)
    ldr(r4, [r3, 0])
    str(r4, [r0, 4])
    label(FORWARDREF)
    data(4, 77, 66, 55)
    label(END)

a = array.array('i', [0, 0])
f(a)
print(a[0])
print(a[1])
