try:
    extra_coverage
except NameError:
    print("SKIP")
    import sys
    sys.exit()

extra_coverage()
