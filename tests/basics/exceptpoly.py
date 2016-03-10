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

