CONDITIONAL_BRANCH_TEMPLATE = """
@micropython.asm_xtensa
def test_branch(a2):
    movi(a3, 1)
    b{}i(a2, {}, END)
    movi(a3, 0)
    label(END)
    mov(a2, a3)
"""

VALID_IMMEDIATES = (-1, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 32, 64, 128, 256)

for condition in ("eq", "ge", "geu", "lt", "ltu", "ne"):
    print(condition)
    for immediate in VALID_IMMEDIATES:
        try:
            exec(CONDITIONAL_BRANCH_TEMPLATE.format(condition, immediate))
        except SyntaxError as error:
            print(error)
            raise SystemExit
        print(test_branch(immediate - 1), test_branch(immediate), test_branch(immediate + 1))
