# Test REPL paste mode functionality

# Basic paste mode with a simple function
{\x05}
def hello():
    print('Hello from paste mode!')
hello()
{\x04}

# Paste mode with multiple indentation levels
{\x05}
def calculate(n):
    if n > 0:
        for i in range(n):
            if i % 2 == 0:
                print(f'Even: {i}')
            else:
                print(f'Odd: {i}')
    else:
        print('n must be positive')

calculate(5)
{\x04}

# Paste mode with blank lines
{\x05}
def function_with_blanks():
    print('First line')
    
    print('After blank line')
    
    
    print('After two blank lines')

function_with_blanks()
{\x04}

# Paste mode with class definition and multiple methods
{\x05}
class TestClass:
    def __init__(self, value):
        self.value = value
    
    def display(self):
        print(f'Value is: {self.value}')
    
    def double(self):
        self.value *= 2
        return self.value

obj = TestClass(21)
obj.display()
print(f'Doubled: {obj.double()}')
obj.display()
{\x04}

# Paste mode with exception handling
{\x05}
try:
    x = 1 / 0
except ZeroDivisionError:
    print('Caught division by zero')
finally:
    print('Finally block executed')
{\x04}

# Cancel paste mode with Ctrl-C
{\x05}
print('This should not execute')
{\x03}

# Normal REPL still works after cancelled paste
print('Back to normal REPL')

# Paste mode with syntax error
{\x05}
def bad_syntax(:
    print('Missing parameter')
{\x04}

# Paste mode with runtime error
{\x05}
def will_error():
    undefined_variable
    
will_error()
{\x04}

# Final test to show REPL is still functioning
1 + 2 + 3
