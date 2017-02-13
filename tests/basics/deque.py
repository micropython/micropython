# test collections.deque
import sys
try:
    from collections import deque
except ImportError:
    try:
        import micropython
        from ucollections import deque
    except ImportError:
        print("SKIP")
        sys.exit()

d = deque('ghi')                                    # make a new deque with three items
for elem in d:                                      # iterate over the deque's elements
    print(elem.upper())

d.append('j')                                       # add a new entry to the right side
d.appendleft('f')                                   # add a new entry to the left side
print(str(d) == "deque(['f', 'g', 'h', 'i', 'j'])") # show the representation of the deque
print(d.pop() == 'j')                               # return and remove the rightmost item
print(d.popleft() == 'f')                           # return and remove the leftmost item
print(str(list(d)) == "['g', 'h', 'i']")            # list the contents of the deque
print(d[0] == 'g')                                  # peek at leftmost item
print(d[-1] == 'i')                                 # peek at rightmost item
print(str(list(reversed(d))) == "['i', 'h', 'g']")  # list the contents of a deque in reverse
print('h' in d)                                     # search the deque
d.extend('jkl')                                     # add multiple elements at once
print(str(d) == "deque(['g', 'h', 'i', 'j', 'k', 'l'])")
d.rotate(1)                                          # right rotation
print(str(d) == "deque(['l', 'g', 'h', 'i', 'j', 'k'])")
d.rotate(-1)                                         # left rotation
print(str(d) == "deque(['g', 'h', 'i', 'j', 'k', 'l'])")
nd = deque(reversed(d))                              # make a new deque in reverse order
print(str(nd) == "deque(['l', 'k', 'j', 'i', 'h', 'g'])")
d.clear()                                            # empty the deque
try:
    d.pop()                                          # cannot pop from an empty deque
except IndexError as ie:
    print(ie)
d.extendleft('abc')                                  # extendleft() reverses the input order
print(str(d) == "deque(['c', 'b', 'a'])")
d.reverse()
print(str(d) == "deque(['a', 'b', 'c'])")            # the contents of a deque in reverse

if sys.implementation.name == "micropython":
    dc = d.copy()                                    # create a copy of deque
    assert (str(d) == str(dc))

    dml = deque('ghi', 3)
    micropython.heap_lock()
    dml.append('j')
    dml.appendleft('f')
    micropython.heap_unlock()
