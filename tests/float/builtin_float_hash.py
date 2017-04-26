# test builtin hash function with float args

# these should hash to an integer with a specific value
for val in (
    '0.0',
    '1.0',
    '2.0',
    '-12.0',
    '12345.0',
    ):
    print(val, hash(float(val)))

# just check that these values are hashable
for val in (
    '0.1',
    '-0.1',
    '10.3',
    'inf',
    '-inf',
    'nan',
    ):
    print(val, type(hash(float(val))))
