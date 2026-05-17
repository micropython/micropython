try:
    raise MemoryError
except Exception:
    print("Caught MemoryError via Exception")

try:
    raise MemoryError
except MemoryError:
    print("Caught MemoryError")

try:
    raise NameError
except Exception:
    print("Caught NameError via Exception")

try:
    raise NameError
except NameError:
    print("Caught NameError")

try:
    raise NotImplementedError
except RuntimeError:
    print("Caught NotImplementedError via RuntimeError")

try:
    raise NotImplementedError
except NotImplementedError:
    print("Caught NotImplementedError")

try:
    raise OSError
except Exception:
    print("Caught OSError via Exception")

try:
    raise OSError
except OSError:
    print("Caught OSError")

try:
    raise OverflowError
except ArithmeticError:
    print("Caught OverflowError via ArithmeticError")

try:
    raise OverflowError
except OverflowError:
    print("Caught OverflowError")

try:
    raise RuntimeError
except Exception:
    print("Caught RuntimeError via Exception")

try:
    raise RuntimeError
except RuntimeError:
    print("Caught RuntimeError")

try:
    raise SyntaxError
except Exception:
    print("Caught SyntaxError via Exception")

try:
    raise SyntaxError
except SyntaxError:
    print("Caught SyntaxError")

try:
    raise TypeError
except Exception:
    print("Caught TypeError via Exception")

try:
    raise TypeError
except TypeError:
    print("Caught TypeError")

try:
    raise ValueError
except Exception:
    print("Caught ValueError via Exception")

try:
    raise ValueError
except ValueError:
    print("Caught ValueError")

try:
    raise ZeroDivisionError
except ArithmeticError:
    print("Caught ZeroDivisionError via ArithmeticError")

try:
    raise ZeroDivisionError
except ZeroDivisionError:
    print("Caught ZeroDivisionError")
