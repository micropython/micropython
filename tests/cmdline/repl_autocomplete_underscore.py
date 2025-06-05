# Test REPL autocompletion filtering of underscore attributes
# This test is designed to work with frozen code when run with --via-mpy

class TestClass:
    def __init__(self):
        self.public_attr = 1
        self._private_attr = 2
        self.__very_private = 3
    
    def public_method(self):
        pass
    
    def _private_method(self):
        pass
    
    @property
    def public_property(self):
        return 42
    
    @property  
    def _private_property(self):
        return 99

# Create an instance
obj = TestClass()

# Test tab completion on the instance
# The tab character after `obj.` below triggers the completion
obj.	t	
