# test machine module

try:
    try:
        import umachine as machine
    except ImportError:
        import machine
    machine.mem8
except:
    print("SKIP")
    raise SystemExit

print(machine.mem8)

try:
    machine.mem16[1]
except ValueError:
    print("ValueError")

try:
    machine.mem16[1] = 1
except ValueError:
    print("ValueError")

try:
    del machine.mem8[0]
except TypeError:
    print("TypeError")

try:
    machine.mem8[0:1]
except TypeError:
    print("TypeError")

try:
    machine.mem8[0:1] = 10
except TypeError:
    print("TypeError")

try:
    machine.mem8["hello"]
except TypeError:
    print("TypeError")

try:
    machine.mem8["hello"] = 10
except TypeError:
    print("TypeError")
