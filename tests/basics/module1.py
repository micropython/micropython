# test behaviour of module objects

# this module should always exist
import __main__

# print module
print(repr(__main__).startswith("<module '__main__'"))

# store new attribute
__main__.x = 1

# delete attribute
del __main__.x
