try:
    raise ArithmeticError
except Exception:
    print("Caught ArithmeticError via Exception")

try:
    raise ArithmeticError
except ArithmeticError:
    print("Caught ArithmeticError")

try:
    raise AssertionError
except Exception:
    print("Caught AssertionError via Exception")

try:
    raise AssertionError
except AssertionError:
    print("Caught AssertionError")

try:
    raise AttributeError
except Exception:
    print("Caught AttributeError via Exception")

try:
    raise AttributeError
except AttributeError:
    print("Caught AttributeError")

try:
    raise EOFError
except Exception:
    print("Caught EOFError via Exception")

try:
    raise EOFError
except EOFError:
    print("Caught EOFError")

try:
    raise Exception
except BaseException:
    print("Caught Exception via BaseException")

try:
    raise Exception
except Exception:
    print("Caught Exception")

try:
    raise ImportError
except Exception:
    print("Caught ImportError via Exception")

try:
    raise ImportError
except ImportError:
    print("Caught ImportError")

try:
    raise IndentationError
except SyntaxError:
    print("Caught IndentationError via SyntaxError")

try:
    raise IndentationError
except IndentationError:
    print("Caught IndentationError")

try:
    raise IndexError
except LookupError:
    print("Caught IndexError via LookupError")

try:
    raise IndexError
except IndexError:
    print("Caught IndexError")

try:
    raise KeyError
except LookupError:
    print("Caught KeyError via LookupError")

try:
    raise KeyError
except KeyError:
    print("Caught KeyError")

try:
    raise LookupError
except Exception:
    print("Caught LookupError via Exception")

try:
    raise LookupError
except LookupError:
    print("Caught LookupError")
