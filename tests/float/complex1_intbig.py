# test basic complex number functionality

# convert bignum to complex on rhs
ans = 1j + (1 << 70)
print("%.5g %.5g" % (ans.real, ans.imag))
