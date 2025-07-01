# Check that we have sensical bytecode offsets in function.__code__.co_lines

def f1(x, y, obj, obj2, obj3):
    a = x + y # line 4: bc+4 line+4
    b = x - y # line 5: bc+4 line+1
    # line 6
    # line 7
    # line 8
    # line 9
    # line 10
    # line 11
    # line 12
    # line 13
    # line 14
    # line 15
    # line 16
    # line 17
    # line 18
    # line 19
    c = a * b # line 20: bc+4 line+15
    obj.a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t.u.v.w.x.y.z.fun() # line 21: bc+31 line+1; bc+27 line+0
    # line 22
    # line 23
    # line 24: bc+0 line+3
    # line 25
    # line 26
    # line 27: bc+0 line+3
    # line 28
    # line 29
    obj2.a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t.u.v.w.x.y.z.fun() # line 30: bc+31 line+3; bc+27 line+0
    # line 31
    # line 32
    # line 33: bc+0 line+3
    # line 34
    # line 35
    # line 36
    # line 37
    # line 38
    # line 39
    # line 40
    # line 41
    # line 42
    # line 43
    # line 44
    # line 45
    # line 46
    # line 47
    # line 48
    # line 49
    # line 50
    # line 51
    # line 52
    # line 53
    # line 54
    # line 55
    # line 56
    # line 57
    # line 58
    # line 59
    return obj3.a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t.u.v.w.x.y.z.fun() # line 60: bc+31 line+27; bc+27 line+0

def f2(x, y):
    a = x + y # line 63
    b = x - y # line 64
    return a * b # line 65

try:
    f1.__code__.co_lines
except AttributeError:
    print("SKIP")
    raise SystemExit

print("f1")
for start, end, line_no in f1.__code__.co_lines():
    print("line {} start: {}".format(line_no, start))
    print("line {} end: {}".format(line_no, end))

print("f2")
for start, end, line_no in f2.__code__.co_lines():
    print("line {} start: {}".format(line_no, start))
    print("line {} end: {}".format(line_no, end))
