try:
    import random

    random.random
except (ImportError, AttributeError):
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
