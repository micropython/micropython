# Test nesting of f-strings within f-strings.

x = 1

# 2-level nesting, with padding.
print(f"a{f'b{x:2}c':>5}d")

# 4-level nesting using the different styles of quotes.
print(f"""a{f'''b{f"c{f'd{x}e'}f"}g'''}h""")
