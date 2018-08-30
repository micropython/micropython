try:
    str.count
except AttributeError:
    print("SKIP")
    raise SystemExit

# mad.py
# Alf Clement 27-Mar-2014
#
zero=0
three=3
print("1")
print("2")
print(three)
print("{}".format(4))
five=25//5
print(int(five))
j=0
for i in range(4):
	j += i
print(j)
print(3+4)
try:
	a=4//zero
except:
	print(8)
print("xxxxxxxxx".count("x"))
def ten():
	return 10
print(ten())
a=[]
for i in range(13):
	a.append(i)
print(a[11])	
print(a[-1])
str="0123456789"
print(str[1]+str[3])
def p(s):
	print(s)
p("14")
p(15)
class A:
	def __init__(self):
		self.a=16
	def print(self):
		print(self.a)
	def set(self,b):
		self.a=b
a=A()
a.print()
a.set(17)
a.print()
b=A()
b.set(a.a + 1)
b.print()
for i in range(20):
	pass
print(i)
if 20 > 30:
	a="1"
else:
	a="2"
if 0 < 4:
	print(a+"0")
else:
	print(a+"1")
a=[20,21,22,23,24]
for i in a:
	if i < 21:
		continue
	if i > 21:
		break
	print(i)
b=[a,a,a]
print(b[1][2])
print(161//7)
a=24
while True:
	try:
		def gcheck():
			global a
			print(a)	
		gcheck()
		class c25():
			x=25
		x=c25()
		print(x.x)
		raise 
	except:
		print(26)
	print(27+zero)
	break
print(28)
k=29
def f():
	global k
	k = yield k
print(next(f()))
while True:
	k+= 1
	if k < 30:
		continue
	break
print(k)
for i in [1,2,3]:
	class A():
		def __init__(self, c):
			self.a = i+10*c
	b = A(3)
	print(b.a)
print(34)
p=0
for i in range(35, -1, -1):
	print(i)
	p = p + 1
	if p > 0:
		break
p=36
while p == 36:
	print(p)
	p=37
print(p)
for i in [38]:
	print(i)
print(int(exec("def foo(): return 38") == None)+foo())
d = {}
exec("def bar(): return 40", d)
print(d["bar"]())
def fib2(n):
	result = []
	a, b = 0, 1
	while a < n:
		result.append(a)
		a, b = b, a+b
	return result
print(fib2(100)[-2]-14)
Answer={}
Answer["ForAll"]=42
print(Answer["ForAll"])
i = 43
def f(i=i):
    print(i)
i = 44
f()
print(i)
while True:
	try:
		if None != True:
			print(45)
			break
		else:
			print(0)
	except:
		print(0)
print(46)
print(46+1)
def u(p):
	if p > 3:
		return 3*p
	else:
		return u(2*p)-3*u(p)
print(u(16))
def u49():
	return 49
print(u49())
