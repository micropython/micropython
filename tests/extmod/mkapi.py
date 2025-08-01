import mkapitest

rh = mkapitest.NewRgn()
r = rh[0]
print(type(rh), type(r))
print(r.rgnBBox.top)
print(r.rgnBBox.left)
print(r.rgnBBox.right)
print(r.rgnBBox.bottom)

mkapitest.OffsetRgn(rh, 3, 5)
print(r.rgnBBox.top)
print(r.rgnBBox.left)
print(r.rgnBBox.right)
print(r.rgnBBox.bottom)

w = mkapitest.NewWindow()
print(type(w))
print(w.portRect.top)
print(w.portRect.left)
print(w.portRect.right)
print(w.portRect.bottom)

r = mkapitest.Rect(1, 2, 3, 4)
print(r)
print(repr(r))
r = mkapitest.Rect(top=10, left=20, right=30, bottom=40)
print(repr(r))
r = mkapitest.Rect(100, 200, right=300, bottom=400)
print(repr(r))

try:
    r = mkapitest.Rect(1000, 2000, 3000, right=4000, bottom=5000)
    print(repr(r))
except Exception as e:
    print(type(e))
