CMMV_TEMPLATE = """
@micropython.asm_rv32
def t():
    cm_mv{}({}, {})
"""

CMMV_TESTS = (
    ("s0", "s9", False),
    ("s0", "s0", False),
    ("s1", "s1", False),
    ("s9", "s10", False),
    ("s0", "s1", True),
    ("s1", "s2", True),
    ("s2", "s3", True),
    ("s3", "s4", True),
)


def cmmv_test(op, tests):
    passed = True
    for lhs, rhs, success in tests:
        try:
            exec(CMMV_TEMPLATE.format("a01s", lhs, rhs))
            if success is False:
                print("cm.mv{} {} {} syntax fail".format(op, lhs, rhs))
                passed = False
        except SyntaxError:
            if success is True:
                print("cm.mv{} {} {} syntax fail".format(op, lhs, rhs))
                passed = False
    if passed:
        print("cm.mv{} syntax pass".format(op))


cmmv_test("a01s", CMMV_TESTS)
cmmv_test("sa01", CMMV_TESTS)


CMPP_TEMPLATE = """
@micropython.asm_rv32
def t():
    cm_push({}, {})
    cm_pop{}({}, {})
"""


CMPP_TESTS = (
    ("", 0, 0, False),
    ("ra", 0, 0, False),
    ("ra, s0", 0, 0, False),
    ("ra, s0-s1", 0, 0, False),
    ("ra, s0-s10", 0, 0, False),
    ("{}", 0, 0, False),
    ("{t0}", 0, 0, False),
    ("{s0}", 0, 0, False),
    ("{s0-s1}", 0, 0, False),
    ("{ra, s1-s0}", 0, 0, False),
    ("{ra, s0-s10}", 0, 0, False),
    ("{ra}", 16, -16, False),
    ("{ra, s0}", -20, 20, False),
    ("{ra}", 0, 0, True),
    ("{ra, s0}", 0, 0, True),
    ("{ra, s0-s1}", 0, 0, True),
    ("{ra, s0-s5}", 0, 0, True),
    ("{ra, s0-s11}", 0, 0, True),
    ("{ra}", -16, 16, True),
    ("{ra, s0}", -16, 16, True),
    ("{ra, s0-s1}", -16, 16, True),
    ("{ra}", -32, 32, True),
    ("{ra, s0}", -32, 32, True),
    ("{ra, s0-s1}", -32, 32, True),
    ("{ra}", -48, 48, True),
    ("{ra, s0}", -48, 48, True),
    ("{ra, s0-s1}", -48, 48, True),
)


def cmpp_test(op, tests):
    passed = True
    for lhs, rhs_in, rhs_out, success in tests:
        try:
            exec(CMPP_TEMPLATE.format(lhs, rhs_in, op, lhs, rhs_out))
            if success is False:
                print("cm.push/cm.pop{} {} {} {} syntax fail".format(op, lhs, rhs_in, rhs_out))
                passed = False
        except SyntaxError:
            if success is True:
                print("cm.push/cm.pop{} {} {} {} syntax fail".format(op, lhs, rhs_in, rhs_out))
                passed = False
    if passed:
        print("cm.push/cm.pop{} syntax pass".format(op))


cmpp_test("", CMPP_TESTS)
cmpp_test("ret", CMPP_TESTS)
cmpp_test("retz", CMPP_TESTS)


CM_POP_TEMPLATE = """
@micropython.asm_rv32
def t(a0):
    mv(t1, s0)
    mv(a1, a0)
    mv(s0, a0)
    cm_push({{ra, s0}}, {})
    add(s0, a1, a1)
    cm_pop({{ra, s0}}, {})
    bne(s0, a0, fail)
    li(a0, 1)
    c_j(end)
    label(fail)
    li(a0, 0)
    label(end)
print("cm.push/cm.pop {}", t(0x1234) == 1)
"""

for stack_adj in 0, 16, 32, 48:
    exec(CM_POP_TEMPLATE.format(-stack_adj, stack_adj, stack_adj))


CM_POPRET_TEMPLATE = """
@micropython.asm_rv32
def t(a0):
    mv(t0, ra)
    mv(a1, s0)
    jal(ra, clobber)
    mv(ra, t0)
    bne(s0, a1, fail)
    li(a0, 1)
    c_j(end)
    label(fail)
    li(a0, 0)
    label(end)
    c_jr(ra)
    label(clobber)
    cm_push({{ra, s0}}, {})
    mv(s0, a0)
    cm_popret({{ra, s0}}, {})
print("cm.push/cm.popret {}", t(0x1234) == 1)
"""

for stack_adj in 0, 16, 32, 48:
    exec(CM_POPRET_TEMPLATE.format(-stack_adj, stack_adj, stack_adj))


CM_POPRETZ_TEMPLATE = """
@micropython.asm_rv32
def t(a0):
    mv(t0, ra)
    mv(a1, s0)
    jal(ra, clobber)
    mv(ra, t0)
    bne(a0, zero, fail)
    bne(s0, a1, fail)
    li(a0, 1)
    c_j(end)
    label(fail)
    li(a0, 0)
    label(end)
    c_jr(ra)
    label(clobber)
    cm_push({{ra, s0}}, {})
    mv(s0, a0)
    cm_popretz({{ra, s0}}, {})
print("cm.push/cm.popretz {}", t(0x1234) == 1)
"""


for stack_adj in 0, 16, 32, 48:
    exec(CM_POPRETZ_TEMPLATE.format(-stack_adj, stack_adj, stack_adj))


REGLIST_TEMPLATE = """
@micropython.asm_rv32
def t():
    li(t6, 0)
{save}
    cm_push({reglist}, 0)
{trash}
    cm_pop({reglist}, 0)
{compare}
    c_j(restore)
    label(fail)
    li(t6, 1)
    label(restore)
{restore}
    mv(a0, t6)
print("reglist {reglist}", t() == 0)
"""

REG_MAP = [
    ("ra", "a0", "{ra}"),
    ("s0", "a1", "{ra,s0}"),
    ("s1", "a2", "{ra,s0-s1}"),
    ("s2", "a3", "{ra,s0-s2}"),
    ("s3", "a4", "{ra,s0-s3}"),
    ("s4", "a5", "{ra,s0-s4}"),
    ("s5", "a6", "{ra,s0-s5}"),
    ("s6", "a7", "{ra,s0-s6}"),
    ("s7", "t0", "{ra,s0-s7}"),
    ("s8", "t1", "{ra,s0-s8}"),
    ("s9", "t2", "{ra,s0-s9}"),
    ("s10", "t3", "{ra,s0-s11}"),
    ("s11", "t4", "{ra,s0-s11}"),
]

for i in range(len(REG_MAP)):
    save = ""
    trash = ""
    compare = ""
    restore = ""
    _, _, reglist = REG_MAP[i]
    for j in range(i + 1):
        r1, r2, _ = REG_MAP[j]
        save += "    mv({}, {})\n".format(r2, r1)
        trash += "    li({}, {})\n".format(r1, (0x1111_1111 * (j + 1)) & 0xFFFF_FFFF)
        compare += "    bne({}, {}, fail)\n".format(r2, r1)
        restore += "    mv({}, {})\n".format(r1, r2)
    exec(
        REGLIST_TEMPLATE.format(
            save=save, trash=trash, compare=compare, restore=restore, reglist=reglist
        )
    )


@micropython.asm_rv32
def test_cm_mva01s(a0, a1):
    cm_push({ra, s0 - s11}, 0)
    mv(s4, a0)
    mv(s5, a1)
    li(a0, 0)
    li(a1, 0)
    cm_mva01s(s4, s5)
    bne(a0, s4, fail)
    bne(a1, s5, fail)
    li(a0, 1)
    c_j(end)
    label(fail)
    li(a0, 0)
    label(end)
    cm_pop({ra, s0 - s11}, 0)


print("cm.mva01s", test_cm_mva01s(100, 200) == 1)


@micropython.asm_rv32
def test_cm_mvsa01(a0, a1):
    mv(t0, s6)
    mv(t1, s7)
    cm_push({ra, s0 - s11}, 0)
    li(s6, 0x12345678)
    li(s7, 0x87654321)
    cm_mvsa01(s6, s7)
    bne(a0, s6, fail)
    bne(a1, s7, fail)
    li(a0, 1)
    c_j(end)
    label(fail)
    li(a0, 0)
    label(end)
    cm_pop({ra, s0 - s11}, 0)


print("cm.mvsa01", test_cm_mvsa01(100, 200) == 1)
