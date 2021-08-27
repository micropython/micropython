f = open("stm32_hal_conf.h")

defines = {}

for line in f:
    if "define HAL_" in line:
        parts = line.split()
        index = len(parts)
        for i, part in enumerate(parts):
            if part.startswith("HAL_"):
                index = i
        define = parts[index]
        if define not in defines:
            defines[define] = index == 1

keys = sorted(defines.keys())
for k in keys:
    if not defines[k]:
        print("// ", end="")

    print("#define", k)
