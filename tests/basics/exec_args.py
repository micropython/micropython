# test various globals and locals arguments to exec()

try:
	exec('print(1)', 'foo')
except TypeError:
	print('TypeError')

foo = 11
exec('print(foo)')
exec('print(foo)', None)
exec('print(foo)', {'foo':3}, None)
exec('print(foo)', None, {'foo':3})
exec('print(foo)', {'bar':3}, locals())

try:
	exec('print(foo)', {'bar':3}, None)
except NameError:
	print('NameError')

exec('print(foo)', None, {'foo':3})
exec('print(foo)', None, {'bar':3})
