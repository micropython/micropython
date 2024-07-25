

class Indent:
    class IndentLabel:
        def __init__(self, parent, label, **kw):
            self.parent = parent
            self.label = label
            self.kw = kw
        def __enter__(self):
            print(self.parent, self.label, "{")
            self.parent.__enter__()
        def __exit__(self, exc_type, exc_value, traceback):
            self.parent.__exit__(exc_type, exc_value, traceback)
            if exc_value:
                print(self.parent, "} ->", repr(exc_value))
            else:
                print(self.parent, "}", **self.kw )

    def __init__(self, init_count=0, multiplier=2):
        self.count = init_count
        self.multiplier = multiplier
    def __str__(self):
        return " "*(self.multiplier*self.count)
    def __call__(self, *a, **kw):
        return self.IndentLabel(self, *a, **kw)
    def __enter__(self):
        self.count += 1
    def __exit__(self, exc_type, exc_value, traceback):
        self.count -= 1

indent = Indent(multiplier=2)

def repr_class(cls):
    return f"<class '{cls.__name__}'>"
def repr_super(s):
    return repr(s)

try:
    with indent("AAA", end=" = "):
        class AAA:
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("AAA.__init__"):
                    s = super(AAA, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("AAA.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(AAA, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<AAA object>"
    
    print(repr_class(AAA))
except Exception as e:
    pass

try:
    with indent("AAB", end=" = "):
        class AAB:
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("AAB.__init__"):
                    s = super(AAB, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("AAB.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(AAB, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<AAB object>"
    
    print(repr_class(AAB))
except Exception as e:
    pass

try:
    with indent("ABA", end=" = "):
        class ABA:
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("ABA.__init__"):
                    s = super(ABA, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("ABA.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(ABA, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<ABA object>"
    
    print(repr_class(ABA))
except Exception as e:
    pass

try:
    with indent("ABB", end=" = "):
        class ABB:
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("ABB.__init__"):
                    s = super(ABB, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("ABB.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(ABB, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<ABB object>"
    
    print(repr_class(ABB))
except Exception as e:
    pass

try:
    with indent("AAx", end=" = "):
        class AAx(AAA, AAB):
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("AAx.__init__"):
                    s = super(AAx, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("AAx.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(AAx, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<AAx object>"
    
    print(repr_class(AAx))
except Exception as e:
    pass

try:
    with indent("ABx", end=" = "):
        class ABx(ABA, ABB):
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("ABx.__init__"):
                    s = super(ABx, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("ABx.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(ABx, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<ABx object>"
    
    print(repr_class(ABx))
except Exception as e:
    pass

try:
    with indent("Axx", end=" = "):
        class Axx(AAx, ABx):
            """Class Inheritance Tree to test MRO Traversal Order............................................................"""
            
            def __init__(self) -> None:
                global indent
                with indent("Axx.__init__"):
                    s = super(Axx, self)
                    print(indent, "super =", repr_super(s))
                    s.__init__()
            
            def __init_subclass__(cls) -> None:
                global indent
                with indent("Axx.__init_subclass__"):
                    print(indent, "cls =", repr_class(cls))
                    s = super(Axx, cls)
                    print(indent, "super =", repr_super(s))
                    s.__init_subclass__()      
            
            def __repr__(self) -> str:
                return "<Axx object>"
    
    print(repr_class(Axx))
except Exception as e:
    pass




try:
    """...................................................................................................."""
    with indent("AAA()", end=" = "):
        aaa = AAA()
    print(repr(aaa))
except Exception as e:
    pass

try:
    """...................................................................................................."""
    with indent("AAB()", end=" = "):
        aab = AAB()
    print(repr(aab))
except Exception as e:
    pass

try:
    """...................................................................................................."""
    with indent("ABA()", end=" = "):
        aba = ABA()
    print(repr(aba))
except Exception as e:
    pass

try:
    """...................................................................................................."""
    with indent("ABB()", end=" = "):
        abb = ABB()
    print(repr(abb))
except Exception as e:
    pass

try:
    """...................................................................................................."""
    with indent("AAx()", end=" = "):
        aax = AAx()
    print(repr(aax))
except Exception as e:
    pass

try:
    """...................................................................................................."""
    with indent("ABx()", end=" = "):
        abx = ABx()
    print(repr(abx))
except Exception as e:
    pass

try:
    """...................................................................................................."""
    with indent("Axx()", end=" = "):
        axx = Axx()
    print(repr(axx))
except Exception as e:
    pass
