# test round() with integer values and 
# arbitary length integers.

tests = [
    (False,), (True,),
    (0.5, False), (1.5, False), (1, False), (1, True),
    (0,), (1,), (-1,), (10,),
    (124, -1), (125, -1), (126, -1),
    (5, -1), (15, -1), (25, -1),
    (12345, 0), (12345, -1), (12345, 1),
    (-1234, 0), (-1234, -1), (-1234, 1),
    (2**65, 0), (2**65, -1), (2**65, 1),
    (-2**65, 0), (-2**65, -1), (-2**65, 1),
]
for t in tests:
    print(round(*t))
