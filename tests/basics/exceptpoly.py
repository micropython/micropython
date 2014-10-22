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

#try:
#    raise BufferError
#except Exception:
#    print("Caught BufferError via Exception")

#try:
#    raise BufferError
#except BufferError:
#    print("Caught BufferError")

#try:
#    raise BytesWarning
#except Warning:
#    print("Caught BytesWarning via Warning")

#try:
#    raise BytesWarning
#except BytesWarning:
#    print("Caught BytesWarning")

#try:
#    raise DeprecationWarning
#except Warning:
#    print("Caught DeprecationWarning via Warning")

#try:
#    raise DeprecationWarning
#except DeprecationWarning:
#    print("Caught DeprecationWarning")

try:
    raise EOFError
except Exception:
    print("Caught EOFError via Exception")

try:
    raise EOFError
except EOFError:
    print("Caught EOFError")

#try:
#    raise EnvironmentError
#except Exception:
#    print("Caught EnvironmentError via Exception")

#try:
#    raise EnvironmentError
#except EnvironmentError:
#    print("Caught EnvironmentError")

try:
    raise Exception
except BaseException:
    print("Caught Exception via BaseException")

try:
    raise Exception
except Exception:
    print("Caught Exception")

#try:
#    raise FloatingPointError
#except ArithmeticError:
#    print("Caught FloatingPointError via ArithmeticError")

#try:
#    raise FloatingPointError
#except FloatingPointError:
#    print("Caught FloatingPointError")

#try:
#    raise FutureWarning
#except Warning:
#    print("Caught FutureWarning via Warning")

#try:
#    raise FutureWarning
#except FutureWarning:
#    print("Caught FutureWarning")

#try:
#    raise IOError
#except Exception:
#    print("Caught IOError via Exception")

#try:
#    raise IOError
#except IOError:
#    print("Caught IOError")

try:
    raise ImportError
except Exception:
    print("Caught ImportError via Exception")

try:
    raise ImportError
except ImportError:
    print("Caught ImportError")

#try:
#    raise ImportWarning
#except Warning:
#    print("Caught ImportWarning via Warning")

#try:
#    raise ImportWarning
#except ImportWarning:
#    print("Caught ImportWarning")

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

#try:
#    raise PendingDeprecationWarning
#except Warning:
#    print("Caught PendingDeprecationWarning via Warning")

#try:
#    raise PendingDeprecationWarning
#except PendingDeprecationWarning:
#    print("Caught PendingDeprecationWarning")

#try:
#    raise ReferenceError
#except Exception:
#    print("Caught ReferenceError via Exception")

#try:
#    raise ReferenceError
#except ReferenceError:
#    print("Caught ReferenceError")

#try:
#    raise ResourceWarning
#except Warning:
#    print("Caught ResourceWarning via Warning")

#try:
#    raise ResourceWarning
#except ResourceWarning:
#    print("Caught ResourceWarning")

try:
    raise RuntimeError
except Exception:
    print("Caught RuntimeError via Exception")

try:
    raise RuntimeError
except RuntimeError:
    print("Caught RuntimeError")

#try:
#    raise RuntimeWarning
#except Warning:
#    print("Caught RuntimeWarning via Warning")

#try:
#    raise RuntimeWarning
#except RuntimeWarning:
#    print("Caught RuntimeWarning")

try:
    raise SyntaxError
except Exception:
    print("Caught SyntaxError via Exception")

try:
    raise SyntaxError
except SyntaxError:
    print("Caught SyntaxError")

#try:
#    raise SyntaxWarning
#except Warning:
#    print("Caught SyntaxWarning via Warning")

#try:
#    raise SyntaxWarning
#except SyntaxWarning:
#    print("Caught SyntaxWarning")

#try:
#    raise SystemError
#except Exception:
#    print("Caught SystemError via Exception")

#try:
#    raise SystemError
#except SystemError:
#    print("Caught SystemError")

#try:
#    raise TabError
#except IndentationError:
#    print("Caught TabError via IndentationError")

#try:
#    raise TabError
#except TabError:
#    print("Caught TabError")

try:
    raise TypeError
except Exception:
    print("Caught TypeError via Exception")

try:
    raise TypeError
except TypeError:
    print("Caught TypeError")

#try:
#    raise UnboundLocalError
#except NameError:
#    print("Caught UnboundLocalError via NameError")

#try:
#    raise UnboundLocalError
#except UnboundLocalError:
#    print("Caught UnboundLocalError")

#try:
#    raise UserWarning
#except Warning:
#    print("Caught UserWarning via Warning")

#try:
#    raise UserWarning
#except UserWarning:
#    print("Caught UserWarning")

try:
    raise ValueError
except Exception:
    print("Caught ValueError via Exception")

try:
    raise ValueError
except ValueError:
    print("Caught ValueError")

#try:
#    raise Warning
#except Exception:
#    print("Caught Warning via Exception")

#try:
#    raise Warning
#except Warning:
#    print("Caught Warning")

try:
    raise ZeroDivisionError
except ArithmeticError:
    print("Caught ZeroDivisionError via ArithmeticError")

try:
    raise ZeroDivisionError
except ZeroDivisionError:
    print("Caught ZeroDivisionError")

