for a in b:
    continue

for a in b:
    try:
        f()
    except:
        continue
    g()

for a in b:
    try:
        f()
        continue
    except:
        g()

for a in b:
    try:
        f()
    except:
        try:
            g()
        except:
            continue

for a in b:
    try:
        f()
    except:
        try:
            g()
            continue
        except:
            h()

for a in b:
    try:
        f()
    except:
        pass
    else:
        continue
    g()
