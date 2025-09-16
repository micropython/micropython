#!/usr/bin/env python3
"""Generate module init function from module definition."""

import re
import sys

def parse_module_def(filename):
    """Parse MODULE_* macros from C file."""
    with open(filename, 'r') as f:
        content = f.read()
    
    # Find module definition block
    module_match = re.search(r'MODULE_BEGIN\((\w+)\)(.*?)MODULE_END\(\)', 
                            content, re.DOTALL)
    if not module_match:
        return None, []
    
    module_name = module_match.group(1)
    module_block = module_match.group(2)
    
    attrs = []
    
    # Parse function definitions
    for match in re.finditer(r'MODULE_FUNCTION\((\w+),\s*(\w+)\)', module_block):
        name, obj = match.groups()
        attrs.append(('function', name, obj))
    
    # Parse constant definitions
    for match in re.finditer(r'MODULE_CONST\((\w+),\s*(.+?)\)', module_block):
        name, value = match.groups()
        attrs.append(('const', name, value.strip()))
    
    return module_name, attrs

def generate_init_function(module_name, attrs):
    """Generate the module init function."""
    lines = []
    lines.append("// Auto-generated module init function")
    lines.append("mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {")
    lines.append("    MP_DYNRUNTIME_INIT_ENTRY")
    lines.append("")
    
    # Generate registration for each attribute
    for attr_type, name, value in attrs:
        if attr_type == 'function':
            lines.append(f"    mp_store_global(MP_QSTR_{name}, MP_OBJ_FROM_PTR(&{value}));")
        elif attr_type == 'const':
            lines.append(f"    mp_store_global(MP_QSTR_{name}, MP_OBJ_NEW_SMALL_INT({value}));")
    
    lines.append("")
    lines.append("    MP_DYNRUNTIME_INIT_EXIT")
    lines.append("}")
    
    return '\n'.join(lines)

def generate_qstrs(attrs):
    """Generate QSTR definitions."""
    qstrs = set()
    for attr_type, name, _ in attrs:
        qstrs.add(name)
    
    # Add any literal QSTRs found in the module
    qstrs.add("Hello_from_autogen_module")
    
    lines = ["// Auto-generated QSTR definitions"]
    for qstr in sorted(qstrs):
        lines.append(f"Q({qstr})")
    
    return '\n'.join(lines)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: generate_module.py <source.c>")
        sys.exit(1)
    
    source_file = sys.argv[1]
    module_name, attrs = parse_module_def(source_file)
    
    if not module_name:
        print("No module definition found")
        sys.exit(1)
    
    # Generate init function
    with open("module_init_generated.h", "w") as f:
        f.write(generate_init_function(module_name, attrs))
    
    # Generate QSTRs
    with open("qstrdefsport.h", "w") as f:
        f.write(generate_qstrs(attrs))
    
    print(f"Generated init function for module '{module_name}' with {len(attrs)} attributes")