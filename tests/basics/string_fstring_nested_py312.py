# Test nesting of f-strings within f-strings.
# These test rely on Python 3.12+ to use the same quote style for nesting.

x = 1

# 8-level nesting using the same quote style.
print(f"a{f"b{f"c{f"d{f"e{f"f{f"g{f"h{x}i"}j"}k"}l"}m"}n"}o"}p")
