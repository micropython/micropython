try:
    exec("@micropython.asm_thumb\ndef l():\n    a = di(a2, a2, -1)")
except SyntaxError as e:
    print(e)
