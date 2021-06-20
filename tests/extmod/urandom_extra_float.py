try:
    import urandom as random
except ImportError:
    try:
        import random
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    random.randint
except AttributeError:
    print("SKIP")
    raise SystemExit

print("random")
for i in range(50):
    assert 0 <= random.random() < 1

print("uniform")
for i in range(50):
    assert 0 <= random.uniform(0, 4) <= 4
    assert 2 <= random.uniform(2, 6) <= 6
    assert -2 <= random.uniform(-2, 2) <= 2
