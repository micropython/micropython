try:
    import ulab
except:
    print('SKIP')
    raise SystemExit

ulab.array([1,2,3])
ulab.array([1,2,3], dtype=ulab.int8)
ulab.array([1,2,3], dtype=ulab.int16)
ulab.array([1,2,3], dtype=ulab.uint8)
ulab.array([1,2,3], dtype=ulab.uint16)
ulab.array([1,2,3], dtype=ulab.float)
ulab.zeros(3)
ulab.ones(3)
a = ulab.linalg.eye(3)
a.shape
a.size
a.itemsize
a.flatten
a.sort()
a.transpose()
a + 0
a + a
a * 0
a * a
a / 1
a / a
a - 0
a - a
+a
-a
a[0]
a[:]
a[0] = 0
a[:] = ulab.zeros((3,3))
a = ulab.linalg.eye(3)
ulab.vector.acos(a)
ulab.vector.acosh(a)
ulab.vector.asin(a)
ulab.vector.asinh(a)
ulab.vector.atan(a)
ulab.vector.atanh(a)
ulab.vector.ceil(a)
ulab.vector.cos(a)
#ulab.vector.cosh(a)
ulab.vector.sin(a)
ulab.vector.sinh(a)
ulab.vector.tan(a)
ulab.vector.tanh(a)
ulab.vector.erf(a)
ulab.vector.erfc(a)
ulab.vector.exp(a)
ulab.vector.expm1(a)
ulab.vector.floor(a)
ulab.vector.gamma(a)
ulab.vector.lgamma(a)
ulab.vector.log(a)
ulab.vector.log2(a)
ulab.vector.sqrt(a)
ulab.linalg.dot(a,a)
ulab.linalg.inv(a)
ulab.linalg.eig(a)
ulab.linalg.det(a)
ulab.filter.convolve(ulab.array([1,2,3]), ulab.array([1,10,100,1000]))
ulab.numerical.linspace(0, 10, num=3)
a = ulab.numerical.linspace(0, 10, num=256, endpoint=True)
ulab.fft.spectrum(a)
p, q = ulab.fft.fft(a)
ulab.fft.ifft(p)
ulab.fft.ifft(p,q)
ulab.numerical.argmin(a)
ulab.numerical.argmax(a)
ulab.numerical.argsort(a)
ulab.numerical.max(a)
ulab.numerical.min(a)
ulab.numerical.mean(a)
ulab.numerical.std(a)
ulab.numerical.diff(a)
#ulab.size(a)
f = ulab.poly.polyfit([1,2,3], 3)
ulab.poly.polyval([1,2,3], [1,2,3])
ulab.numerical.sort(a)
ulab.numerical.flip(a)
ulab.numerical.roll(a, 1)
