# test builtin pow function with float args

print(pow(0.0, 0.0))
print(pow(0, 1.0))
print(pow(1.0, 1))
print(pow(2.0, 3.0))
print(pow(2.0, -4.0))

print(pow(0.0, float('inf')))
print(pow(0.0, float('-inf')))
print(pow(0.0, float('nan')))
