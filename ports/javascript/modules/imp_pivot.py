__name__ = imp.pivot_name
__file__ = imp.pivot_file
print('pivot',__name__,__file__, globals())

exec( compile( imp.pivot_code, __file__, 'exec') , globals(), globals() )
