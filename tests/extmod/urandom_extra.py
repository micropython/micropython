try:
    import urandom as random
except ImportError:
    try:
        import random
    except ImportError:
        import sys
        print("SKIP")
        sys.exit()

try:
    random.randint
except AttributeError:
    import sys
    print('SKIP')
    sys.exit()

print('randrange')
for i in range(50):
    assert 0 <= random.randrange(4) < 4
    assert 2 <= random.randrange(2, 6) < 6
    assert -2 <= random.randrange(-2, 2) < 2
    assert random.randrange(1, 9, 2) in (1, 3, 5, 7)
    assert random.randrange(2, 1, -1) in (1, 2)

# empty range
try:
    random.randrange(0)
except ValueError:
    print('ValueError')

# empty range
try:
    random.randrange(2, 1)
except ValueError:
    print('ValueError')

# zero step
try:
    random.randrange(2, 1, 0)
except ValueError:
    print('ValueError')

# empty range
try:
    random.randrange(2, 1, 1)
except ValueError:
    print('ValueError')

print('randint')
for i in range(50):
    assert 0 <= random.randint(0, 4) <= 4
    assert 2 <= random.randint(2, 6) <= 6
    assert -2 <= random.randint(-2, 2) <= 2

# empty range
try:
    random.randint(2, 1)
except ValueError:
    print('ValueError')

print('choice')
lst = [1, 2, 5, 6]
for i in range(50):
    assert random.choice(lst) in lst

# empty sequence
try:
    random.choice([])
except IndexError:
    print('IndexError')

print('random')
for i in range(50):
    assert 0 <= random.random() < 1

print('uniform')
for i in range(50):
    assert 0 <= random.uniform(0, 4) <= 4
    assert 2 <= random.uniform(2, 6) <= 6
    assert -2 <= random.uniform(-2, 2) <= 2
