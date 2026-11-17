// Test how JavaScript binds self/this when methods are called from Python.

const mp = await (await import(process.argv[2])).loadMicroPython();

// Test accessing and calling JavaScript methods from Python.
mp.runPython(`
import js

# Get the push method to call later on.
push = js.Array.prototype.push

# Create initial array.
ar = js.Array(1, 2)
js.console.log(ar)

# Add an element using a method (should implicitly supply "ar" as context).
print(ar.push(3))
js.console.log(ar)

# Add an element using prototype function, need to explicitly provide "ar" as context.
print(push.call(ar, 4))
js.console.log(ar)

# Add an element using a method with call and explicit context.
print(ar.push.call(ar, 5))
js.console.log(ar)

# Add an element using a different instances method with call and explicit context.
print(js.Array().push.call(ar, 6))
js.console.log(ar)
`);

// Test assigning Python functions to JavaScript objects, and using them like a method.
mp.runPython(`
import js

a = js.Object()
a.meth1 = lambda *x: print("meth1", x)
a.meth1(1, 2)

js.Object.prototype.meth2 = lambda *x: print("meth2", x)
a.meth2(3, 4)
`);
