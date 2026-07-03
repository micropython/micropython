# Format specifiers with nested replacement fields
space = 5
prec = 2
print(f"{3.14:{space}.{prec}}")

space_prec = "5.2"
print(f"{3.14:{space_prec}}")
