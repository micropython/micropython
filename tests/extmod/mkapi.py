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
