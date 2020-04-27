import os
import sys
import astroid
import traceback

top_level = sys.argv[1].strip("/")
stub_directory = sys.argv[2]

if top_level.count("/") == 1:
    top_level, module = top_level.split("/")
    modules = [module]
else:
    modules = os.listdir(top_level)
    modules = sorted(modules)

ok = 0
total = 0
for module in modules:
    module_path = os.path.join(top_level, module)
    if not os.path.isdir(module_path):
        continue
    pyi_lines = []
    for class_file in os.listdir(module_path):
        class_path = os.path.join(module_path, class_file)
        with open(class_path, "r") as f:
            for line in f:
                if line.startswith("//| "):
                    pyi_lines.append(line[4:])

    stub_filename = os.path.join(stub_directory, module + ".pyi")
    print(stub_filename)
    stub_contents = "".join(pyi_lines)
    with open(stub_filename, "w") as f:
        f.write(stub_contents)

    # Validate that the module is a parseable stub.
    total += 1
    try:
        astroid.parse(stub_contents)
        ok += 1
    except astroid.exceptions.AstroidSyntaxError as e:
        e = e.__cause__
        traceback.print_exception(type(e), e, e.__traceback__)
    print()

print(f"{ok} ok out of {total}")
