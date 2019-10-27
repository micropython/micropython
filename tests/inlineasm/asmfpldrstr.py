import uarray as array
@micropython.asm_thumb      # test vldr, vstr
def arrayadd(r0):
    vldr(s0, [r0, 0])
    vldr(s1, [r0, 4])
    vadd(s2, s0, s1)
    vstr(s2, [r0, 8])

z = array.array("f", [2, 4, 10])
arrayadd(z)
print(z[2])

