# Test the marshal module, stressing edge cases.

try:
    import marshal

    (lambda: 0).__code__
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

ftype = type(lambda: 0)

# Test a large function.


def large_function(arg0, arg1, arg2, arg3):
    # Arguments.
    print(arg0, arg1, arg2, arg3)

    # Positive medium-sized integer (still a small-int though).
    print(1234)

    # Negative small-ish integer.
    print(-20)

    # More than 64 constant objects.
    x = (0,)
    x = (1,)
    x = (2,)
    x = (3,)
    x = (4,)
    x = (5,)
    x = (6,)
    x = (7,)
    x = (8,)
    x = (9,)
    x = (10,)
    x = (11,)
    x = (12,)
    x = (13,)
    x = (14,)
    x = (15,)
    x = (16,)
    x = (17,)
    x = (18,)
    x = (19,)
    x = (20,)
    x = (21,)
    x = (22,)
    x = (23,)
    x = (24,)
    x = (25,)
    x = (26,)
    x = (27,)
    x = (28,)
    x = (29,)
    x = (30,)
    x = (31,)
    x = (32,)
    x = (33,)
    x = (34,)
    x = (35,)
    x = (36,)
    x = (37,)
    x = (38,)
    x = (39,)
    x = (40,)
    x = (41,)
    x = (42,)
    x = (43,)
    x = (44,)
    x = (45,)
    x = (46,)
    x = (47,)
    x = (48,)
    x = (49,)
    x = (50,)
    x = (51,)
    x = (52,)
    x = (53,)
    x = (54,)
    x = (55,)
    x = (56,)
    x = (57,)
    x = (58,)
    x = (59,)
    x = (60,)
    x = (61,)
    x = (62,)
    x = (63,)
    x = (64,)

    # Small jump.
    x = 0
    while x < 2:
        print("loop", x)
        x += 1

    # Large jump.
    x = 0
    while x < 2:
        try:
            try:
                try:
                    print
                except Exception as e:
                    print
                finally:
                    print
            except Exception as e:
                print
            finally:
                print
        except Exception as e:
            print
        finally:
            print("loop", x)
        x += 1


code = marshal.dumps(large_function.__code__)
ftype(marshal.loads(code), {"print": print})(0, 1, 2, 3)
