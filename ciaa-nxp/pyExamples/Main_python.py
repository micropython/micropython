import math
import collections

class Clase:
	def __init__(self):
		pass

	def miMetodo(self,arg):
		print("Mi metodo:"+str(arg))

#Test class
c = Clase()
c.miMetodo(56)

# Test bytearray
ba = bytearray()
ba.append(33)
		
# Test slice
values = [100, 200, 300, 400, 500]
slice = values[2:-1]
print(slice)
values = "Hello World"
evens = values[:2]
print(evens)

# Test math
s = math.sqrt(2)
print("sqrt(2)="+str(s))

# Test array
from array import *
x=array('f',[0.0,1.0,2.0])
print(x)

# Test Set
print("Test de Sets")
x = [1, 1, 2, 2, 2, 2, 2, 3, 3]
s = set(x)
print(s)

#Test tuples
a,b = 4,6
print("a:"+str(a)+" b:"+str(b))
a,b,c = 4,5,6
print("a:"+str(a)+" b:"+str(b)+" c:"+str(c))

#Test collection
Person = collections.namedtuple("Person", ["name", "age", "gender"] )
bob = Person(name='Bob', age=30, gender='male')
print(bob)
