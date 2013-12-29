# nested ifs
x = [a for a in l if a if a + 1]
x = [a for a in l if a if a + 1 if a + 2]

# nested for loops
x = [a for a in l for l in ls]
x = [a for ls in lss for l in ls for a in l]
x = [a for a in l for l in ls for ls in lss]

# nested ifs and for loops
x = [a for a in l if a for l in ls if l if a for ls in lss if ls]
