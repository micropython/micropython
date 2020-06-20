import os
import sys
import astroid
import traceback

top_level = sys.argv[1].strip("/")
stub_directory = sys.argv[2]

def convert_folder(top_level, stub_directory):
    ok = 0
    total = 0
    filenames = sorted(os.listdir(top_level))
    pyi_lines = []
    for filename in filenames:
        full_path = os.path.join(top_level, filename)
        file_lines = []
        if os.path.isdir(full_path):
            mok, mtotal = convert_folder(full_path, os.path.join(stub_directory, filename))
            ok += mok
            total += mtotal
        elif filename.endswith(".c"):
            with open(full_path, "r") as f:
                for line in f:
                    if line.startswith("//|"):
                        if line[3] == " ":
                            line = line[4:]
                        elif line[3] == "\n":
                            line = line[3:]
                        else:
                            continue
                        file_lines.append(line)
        elif filename.endswith(".pyi"):
            with open(full_path, "r") as f:
                file_lines.extend(f.readlines())

        # Always put the contents from an __init__ first.
        if filename.startswith("__init__."):
            pyi_lines = file_lines + pyi_lines
        else:
            pyi_lines.extend(file_lines)

    if not pyi_lines:
        return ok, total

    stub_filename = os.path.join(stub_directory, "__init__.pyi")
    print(stub_filename)
    stub_contents = "".join(pyi_lines)
    os.makedirs(stub_directory, exist_ok=True)
    with open(stub_filename, "w") as f:
        f.write(stub_contents)

    # Validate that the module is a parseable stub.
    total += 1
    try:
        tree = astroid.parse(stub_contents)
        for i in tree.body:
            if 'name' in i.__dict__:
                print(i.__dict__['name'])
                for j in i.body:
                    if isinstance(j, astroid.scoped_nodes.FunctionDef):
                        if None in j.args.__dict__['annotations']:
                            print(f"Missing parameter type: {j.__dict__['name']} on line {j.__dict__['lineno']}\n")
                        if j.returns:
                            if 'Any' in j.returns.__dict__.values():
                                print(f"Missing return type: {j.__dict__['name']} on line {j.__dict__['lineno']}")
                    elif isinstance(j, astroid.node_classes.AnnAssign):
                        if 'name' in j.__dict__['annotation'].__dict__:
                            if j.__dict__['annotation'].__dict__['name'] == 'Any':
                                print(f"missing attribute type on line {j.__dict__['lineno']}")

        ok += 1
    except astroid.exceptions.AstroidSyntaxError as e:
        e = e.__cause__
        traceback.print_exception(type(e), e, e.__traceback__)
    print()
    return ok, total

ok, total = convert_folder(top_level, stub_directory)

print(f"{ok} ok out of {total}")

if ok != total:
    sys.exit(total - ok)
