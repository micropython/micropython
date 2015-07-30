try:
    import ure as re
except ImportError:
    import re

regex = r'def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):'
replacement = 'static PyObject*\npy_\\1(void)\n{'
where = 'def myfunc():'

#regex = r'.*(\ )(blu|bianco|rossa).*(blu|bianco|rossa)$'
#replacement = r'\1 colore (\2) ma veramente tanto (\g<2>) colore \3 ma veramente tanto \g<3>'
#where = 'calzino blu e scarpa rossa'

#regex = '(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)'
#replacement = '\\g<0>\n\\g<21>\\g<20>\\g<19>\\g<18>\\g<17>\\g<16>\\g<15>\\g<14>\\g<13>\\g<12>\\g<11>\\g<10>\\9\\8\\7\\6\\5\\4\\3\\2\\1'
#where = 'utsrqponmlkjihgfedcba'

s0 = re.sub(regex, replacement, where)
print(s0, len(s0)) 

if False:
	
	regex = r'.*(\ )(blu|bianco|rossa).*(blu|bianco|rossa)$'
	replacement = r'\1 colore (\2) ma veramente tanto (\g<2>) colore \3 ma veramente tanto \g<3>'
	where = 'calzino blu e scarpa rossa'
	
	for index in range(1):

		p1 = re.compile(regex)
		s1 = p1.sub(replacement, where)
		print(s1, len(s1))

		s2 = re.sub(regex, replacement, where)
		print(s2, len(s1))

		p3 = re.compile(regex)
		s3 = p3.sub(replacement, where, 1)
		print(s3, len(s1))

		def callable(value):
			return value

		s4 = re.sub(
			regex, 
			callable(replacement), 
			where
		)
		print(s4, len(s1))

		p5 = re.compile(regex)
		s5 = p5.sub(
			callable(replacement), 
			where, 
			1
		)
		print(s5, len(s1))



