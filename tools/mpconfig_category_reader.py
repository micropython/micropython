filepath = "../py/circuitpy_mpconfig.mk"
with open(filepath) as fp:
    line = fp.readline()
    cnt = 1
    fullbuild = []
    defon = []
    defoff = []
    while line:
        wordlist = line.split()
        if wordlist:
            if wordlist[-1] == "$(CIRCUITPY_FULL_BUILD)":
                fullbuild.append(wordlist[0])
            elif wordlist[-1] == "0":
                defoff.append(wordlist[0])
            elif wordlist[-1] == "1":
                defon.append(wordlist[0])
        line = fp.readline()
        cnt += 1

    print(str(cnt) + " Lines Read\n")
    print("\nFULL BUILDS ------------------------")
    for string in fullbuild:
        print(string)

    print("\nON BUILDS ------------------------")
    for string in defon:
        print(string)

    print("\nOFF BUILDS ------------------------")
    for string in defoff:
        print(string)
