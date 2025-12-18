# Test REPL autocompletion filtering of underscore attributes

# Start paste mode
{\x05}
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

{\x04}
# Paste executed

# Create an instance
obj = TestClass()

# Test tab completion on the instance
# The tab character after `obj.` and 'a' below triggers the completions
obj.{\x09}{\x09}a{\x09}
