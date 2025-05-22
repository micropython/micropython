# Test REPL autocompletion for underscore attributes in frozen modules

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

# Simulate typing 'asyncio.' and pressing tab
# The tab character after `asyncio.` below triggers the completion
asyncio.	t	
