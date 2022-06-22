import pathlib
paths = pathlib.Path(".").glob("**/*.c")
translate_h = "#include \"supervisor/shared/translate/translate.h\""
for p in paths:
    if "esp-idf" in p:
        continue
    lines = p.read_text().split("\n")
    if "#include \"py/runtime.h\"" in lines and translate_h in lines:
        lines.remove(translate_h)
        p.write_text("\n".join(lines))
