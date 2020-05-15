import os
import sys
import astroid
import traceback

top_level = sys.argv[1].strip("/")

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
    classes = os.listdir(module_path)
    classes = [x for x in sorted(classes) if x.endswith(".c")]
    if classes and classes[-1] == "__init__.c":
        classes.insert(0, classes.pop())
    for class_file in classes:
        class_path = os.path.join(module_path, class_file)
        with open(class_path, "r") as f:
            for line in f:
                if line.startswith("//|"):
                    if line[3] == " ":
                        line = line[4:]
                    elif line[3] == "\n":
                        line = line[3:]
                    else:
                        continue
                    pyi_lines.append(line)

    raw_stubs = [x for x in sorted(classes) if x.endswith(".pyi")]
    if raw_stubs and raw_stubs[-1] == "__init__.pyi":
        raw_stubs.insert(0, raw_stubs.pop())
    for raw_stub in raw_stubs:
        raw_stub_path = os.path.join(module_path, raw_stub)
        with open(raw_stub_path, "r") as f:
            pyi_lines.extend(f.readlines())
    stub_contents = "".join(pyi_lines)

    # Validate that the module is a parseable stub.
    total += 1
    missing_parameter_type = 0
    total_1 = 0
    missing_return_type = 0
    total_2 = 0
    missing_attribute_type = 0
    total_3 = 0
    try:
        tree = astroid.parse(stub_contents)
        for i in tree.body:
            for j in i.body:
                if isinstance(j, astroid.scoped_nodes.FunctionDef):
                    if None in j.args.__dict__['annotations']:
                        missing_parameter_type += 1
                        print(f"Parameter: {j.__dict__['name']} on line {j.__dict__['lineno']}")
                    total_1 += 1
                    if j.returns:
                        if 'Any' in j.returns.__dict__.values():
                            print(f"Return: {j.__dict__['name']} on line {j.__dict__['lineno']}")
                            missing_return_type += 1
                        total_2 += 1
                elif isinstance(j, astroid.node_classes.AnnAssign):
                    if 'Any' == j.__dict__['annotation'].__dict__['name']:
                        missing_attribute_type += 1
                        print(f"attribute on line {j.__dict__['lineno']}")
                    total_3 += 1


        ok += 1
    except astroid.exceptions.AstroidSyntaxError as e:
        e = e.__cause__
        traceback.print_exception(type(e), e, e.__traceback__)
    print()

print(f"{missing_parameter_type} of {total_1} are missing the parameter type")
print(f"{missing_return_type} of {total_2} are missing the return type")
print(f"{missing_attribute_type} of {total_3} are missing the attribute type")


if ok != total:
    sys.exit(total - ok)
