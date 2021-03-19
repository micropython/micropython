# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# This script renders a graph of the CircuitPython rom image.
# It takes the single elf file and uses objdump to get its contents.

import pygraphviz as pgv
import click
import sh

# Replace dashes with underscores
objdump = sh.arm_none_eabi_objdump


def parse_hex(h):
    return int("0x" + h, 0)


BAD_JUMPS = ["UNPREDICTABLE", "_etext"]

SPECIAL_NODE_COLORS = {"main": "pink", "exception_table": "green"}


@click.command()
@click.argument("elf_filename")
def do_all_the_things(elf_filename):
    symbol = None
    last_address = 0
    all_symbols = {}
    symbols_by_debug_address = {}
    symbols_by_memory_address = {}
    symbols_by_linkage_name = {}
    # Gather type info so we know how to treat the disassembly
    debug_dump = objdump("--dwarf=info", elf_filename)
    debug_dump_lines = debug_dump.stdout.decode("utf-8").split("\n")
    symbol_stack = []
    symbol = None
    ignore = False
    min_call_site_param = 0x20000000
    for line in debug_dump_lines:
        if not line:
            continue
        parts = line.split()
        if line[1] == "<":
            if parts[-1] == "0":
                symbol = symbol_stack.pop()
                continue
            debug_type = parts[-1].strip("()")
            ignore = False
            # skip info about function parameters
            if debug_type == "DW_TAG_formal_parameter":
                ignore = True
            depth = int(parts[0].split(">")[0].strip("<"))
            if len(symbol_stack) == (depth - 1) and depth > 0:
                symbol_stack.append(symbol)
            elif symbol and "name" in symbol:
                if symbol["debug_type"] == "DW_TAG_variable":
                    if "start_address" not in symbol:
                        pass
                    else:
                        symbols_by_memory_address[symbol["start_address"]] = symbol
                elif symbol["debug_type"] in [
                    "DW_TAG_member",
                    "DW_TAG_label",
                    "DW_TAG_typedef",
                    "DW_TAG_enumerator",
                    "DW_TAG_enumeration_type",
                    "DW_TAG_base_type",
                    "DW_TAG_structure_type",
                    "DW_TAG_compile_unit",
                    "DW_TAG_union_type",
                ]:
                    # skip symbols that don't end up in memory. the type info is available through the debug address map
                    pass
                else:
                    if symbol["name"] in all_symbols:
                        # print(depth, symbol["name"])
                        # print(symbol)
                        # print(all_symbols[symbol["name"]])
                        # print()
                        pass
                    all_symbols[symbol["name"]] = symbol
            elif (
                symbol
                and symbol["debug_type"] == "DW_TAG_GNU_call_site_parameter"
                and "call_site_value" in symbol
            ):
                parent = -1
                while symbol_stack[parent]["debug_type"] != "DW_TAG_subprogram":
                    parent -= 1
                parent = symbol_stack[parent]

                # Only point to ROM
                addr = symbol["call_site_value"]
                if 0x2000 <= addr < 0x20000000:
                    if "outgoing_pointers" not in parent:
                        parent["outgoing_pointers"] = set()
                    parent["outgoing_pointers"].add(addr)
                    if addr not in symbols_by_memory_address:
                        symbols_by_memory_address[addr] = symbol
                        min_call_site_param = min(addr, min_call_site_param)
                        symbol["name"] = "name{:x}".format(addr)
            address = parse_hex(parts[0].split("<")[-1].strip(">:"))
            symbol = {"debug_address": address, "debug_type": debug_type, "other": []}
            if debug_type == "DW_TAG_structure_type":
                symbol["struct"] = {}
            elif debug_type == "DW_TAG_array_type":
                symbol["subtype"] = None
                symbol["bound_count"] = 0
                symbol["maxlen"] = 0
            elif debug_type == "DW_TAG_subrange_type":
                symbol_stack[-1]["subtype"] = symbol
            symbols_by_debug_address[address] = symbol
        elif ignore:
            continue
        elif line[:4] == "    ":
            tag = parts[1].strip(":")
            if tag == "DW_AT_name":
                symbol["name"] = parts[-1]
            elif tag == "DW_AT_type":
                symbol["type"] = int(parts[-1].strip("<>"), 0)
                if symbol["debug_type"] == "DW_TAG_subrange_type":
                    if not symbol_stack[-1]["subtype"]:
                        symbol_stack[-1]["subtype"] = symbol
                    elif symbol_stack[-1]["subtype"]["type"] == symbol["type"]:
                        second_subtype = True
                    else:
                        raise RuntimeError()
            elif tag == "DW_AT_upper_bound":
                # Skip arrays with length defined by other variables
                if parts[-1][0] != "<":
                    upper_bound = int(parts[-1])
                    if symbol_stack[-1]["bound_count"] > 0:
                        symbol_stack[-1]["maxlen"] *= upper_bound + 1
                    else:
                        symbol_stack[-1]["maxlen"] = upper_bound + 1
                    symbol_stack[-1]["bound_count"] += 1
            elif tag == "DW_AT_byte_size":
                symbol["size"] = int(parts[-1])
            elif tag == "DW_AT_inline":
                symbol["inlined"] = True
            elif tag == "DW_AT_low_pc":
                addr = int(parts[-1], 0)
                symbols_by_memory_address[addr] = symbol
            elif tag == "DW_AT_location":
                if parts[-2] == "(DW_OP_addr:":
                    addr = parse_hex(parts[-1].strip(")"))
                    if addr > 0:
                        symbol["start_address"] = addr
            elif tag == "DW_AT_linkage_name":
                symbol["linkage_name"] = parts[-1]
                symbols_by_linkage_name[symbol["linkage_name"]] = symbol
            elif tag == "DW_AT_data_member_location":
                symbol_stack[-1]["struct"][int(parts[-1])] = symbol
            elif tag == "DW_AT_GNU_call_site_value":
                if parts[-2] == "(DW_OP_addr:":
                    symbol["call_site_value"] = parse_hex(parts[-1].strip(")"))
            else:
                symbol["other"].append(line)
                # print(parts)
                pass
        else:
            # print(line)
            pass

    MEMORY_NONE = 0
    MEMORY_POINTER = 1
    MEMORY_PY_OBJECT = 2

    def get_size(t):
        if "size" in t:
            return t["size"]
        return get_size(symbols_by_debug_address[t["type"]])

    def get_pointer_map(t, depth=0):
        if t["debug_type"] == "DW_TAG_pointer_type":
            return {0: MEMORY_POINTER}
        elif t["debug_type"] in [
            "DW_TAG_const_type",
            "DW_TAG_typedef",
            "DW_TAG_member",
            "DW_TAG_subrange_type",
            "DW_TAG_volatile_type",
        ]:
            if "name" in t and t["name"] == "mp_rom_obj_t":
                return {0: MEMORY_PY_OBJECT}
            return get_pointer_map(symbols_by_debug_address[t["type"]], depth + 1)
        elif t["debug_type"] in ["DW_TAG_base_type", "DW_TAG_enumeration_type"]:
            return {}
        elif t["debug_type"] == "DW_TAG_union_type":
            # skip for now
            return {}
        elif "struct" in t:
            combined_map = {}
            for offset in t["struct"]:
                member = t["struct"][offset]
                submap = get_pointer_map(member)
                for suboffset in submap:
                    combined_map[offset + suboffset] = submap[suboffset]
            return combined_map
        elif "subtype" in t:
            subtype = symbols_by_debug_address[t["type"]]
            pmap = get_pointer_map(subtype, depth + 1)
            size = get_size(subtype)
            expanded_map = {}
            for i in range(t["maxlen"]):
                for offset in pmap:
                    expanded_map[size * i + offset] = pmap[offset]
            return expanded_map
        else:
            print("no recurse", t)
            pass
        return {}

    # Do a second pass to dereference the types
    for symbol_address in symbols_by_memory_address:
        symbol = symbols_by_memory_address[symbol_address]
        if "type" in symbol:
            if symbol["debug_type"] == "DW_TAG_variable":
                symbol["pointer_map"] = get_pointer_map(symbols_by_debug_address[symbol["type"]])
            type_string = []
            t = symbol["type"]
            offset = []
            while t != None:
                t_symbol = symbols_by_debug_address[t]
                t = t_symbol.get("type", None)
                if "name" in t_symbol:
                    type_string.append(t_symbol["name"])
                elif t_symbol["debug_type"] == "DW_TAG_array_type":
                    type_string.append("[]")
                elif t_symbol["debug_type"] == "DW_TAG_pointer_type":
                    type_string.append("*")
                elif t_symbol["debug_type"] == "DW_TAG_const_type":
                    type_string.append("const")
                elif t_symbol["debug_type"] == "DW_TAG_volatile_type":
                    type_string.append("volatile")
                else:
                    # print("  ", t_symbol)
                    pass
            type_string.reverse()
            symbol["type_string"] = " ".join(type_string)
        # print(symbol_name, symbol["debug_type"], symbol.get("type_string", ""))

    # print()
    # print()
    # print(all_symbols["mp_builtin_module_table"])
    # return

    # Gather size and call info
    text_dump = objdump("-Dz", "-j", ".text", elf_filename)
    text_dump_lines = text_dump.stdout.decode("utf-8").split("\n")
    section = None
    symbol = None
    symbol_type = None
    for line in text_dump_lines[4:]:
        if line.startswith("Disassembly of section"):
            section = line.split()[-1].strip(":")
        elif not line:
            if symbol and "end_address" not in symbol:
                symbol["end_address"] = last_address
                symbol["size"] = last_address - symbol["start_address"]
                symbol = None
            continue
        elif line[0].isnumeric():
            symbol_address, symbol_name = line.split()
            symbol_address = parse_hex(symbol_address)
            symbol_name = symbol_name.strip("<>:")
            if symbol_name in symbols_by_linkage_name:
                linked_name = symbol_name
                symbol = symbols_by_linkage_name[symbol_name]
                if "name" in symbol:
                    non_linkage = symbol["name"]
                    if not non_linkage.startswith("__builtin"):
                        symbol_name = non_linkage
                all_symbols[symbol_name] = symbol
                if "name" not in symbol:
                    symbol["name"] = symbol_name
            elif symbol_address in symbols_by_memory_address:
                all_symbols[symbol_name] = symbols_by_memory_address[symbol_address]
                if "name" not in all_symbols[symbol_name]:
                    all_symbols[symbol_name]["name"] = symbol_name
            elif symbol_name not in all_symbols:
                if symbol_name == "nlr_push_tail_var":
                    fake_type = all_symbols["mp_obj_get_type"]["type"]
                    symbol = {
                        "debug_type": "DW_TAG_variable",
                        "name": symbol_name,
                        "type": fake_type,
                    }
                else:
                    print(line)
                    print(symbol_name, symbol_address)
                    symbol = {"debug_type": "DW_TAG_subprogram", "name": symbol_name}
                all_symbols[symbol_name] = symbol
                # raise RuntimeError()

            symbol = all_symbols[symbol_name]
            symbol["start_address"] = symbol_address
            symbols_by_memory_address[symbol_address] = symbol
            symbol["section"] = section

            if symbol["debug_type"] == "DW_TAG_subprogram":
                symbol["outgoing_jumps"] = set()
                symbol["incoming_jumps"] = set()
                symbol_type = None
            elif symbol["debug_type"] == "DW_TAG_variable":
                symbol["outgoing_pointers"] = set()
                symbol_type = symbols_by_debug_address[symbol["type"]]
            all_symbols[symbol_name] = symbol

        elif line[0] == " ":
            parts = line.strip().split()
            last_address = parse_hex(parts[0].strip(":"))

            offset = last_address - symbol["start_address"]
            if "pointer_map" in symbol:
                if offset not in symbol["pointer_map"]:
                    # print(offset, symbol)
                    pass
                else:
                    ref = parse_hex(parts[1])
                    pointer_style = symbol["pointer_map"][offset]
                    if pointer_style == MEMORY_POINTER:
                        symbol["outgoing_pointers"].add(ref & 0xFFFFFFFE)
                    elif pointer_style == MEMORY_PY_OBJECT and ref & 0x3 == 0:
                        symbol["outgoing_pointers"].add(ref)
            if len(parts[1]) == 8 and parts[1][0] == "0":
                addr = parse_hex(parts[1])
                if 0x2000 <= addr < 0x20000000:
                    if "outgoing_pointers" not in symbol:
                        symbol["outgoing_pointers"] = set()
                    symbol["outgoing_pointers"].add(addr)
            elif "<" in line and symbol["debug_type"] == "DW_TAG_subprogram":
                if line[-1] == ">":
                    jump_to = parts[-1].strip("<>").split("+")[0]
                    if "name" not in symbol:
                        print(jump_to)
                        print(symbol)
                    if jump_to != symbol["name"] and jump_to not in BAD_JUMPS:
                        symbol["outgoing_jumps"].add(jump_to)
                        # print(symbol_name, jump_to)
                        if jump_to == "_etext":
                            print(line)
                elif "UNDEFINED" in line:
                    continue
                elif parts[2] == "ldr":
                    continue
                else:
                    print(line)
        else:
            # print(line)
            pass

    # print()
    print(hex(min_call_site_param))
    print(all_symbols["exception_table"])
    # return

    print("converting outgoing pointers to names")

    # Convert outgoing pointers to names from addresses
    for symbol_name in all_symbols:
        symbol = all_symbols[symbol_name]
        if "outgoing_pointers" not in symbol:
            continue
        converted = set()
        for outgoing in symbol["outgoing_pointers"]:
            if outgoing in symbols_by_memory_address:
                outgoing = symbols_by_memory_address[outgoing]
                # print(outgoing)
                if outgoing["debug_type"] in ["DW_TAG_GNU_call_site", "DW_TAG_lexical_block"]:
                    continue
                if outgoing["name"] == "audioio_wavefile_type":
                    print(outgoing)
                converted.add(outgoing["name"])
        symbol["outgoing_pointers"] = converted

    print("linking back")
    # Link back
    for symbol_name in all_symbols:
        symbol = all_symbols[symbol_name]
        if "outgoing_jumps" in symbol:
            for outgoing in symbol["outgoing_jumps"]:
                if outgoing not in all_symbols:
                    # print(outgoing, symbol_name)
                    continue
                # print(all_symbols[outgoing], symbol_name)

                referenced_symbol = all_symbols[outgoing]
                if "incoming_jumps" not in referenced_symbol:
                    # print(symbol_name, "->", outgoing)
                    referenced_symbol["incoming_jumps"] = set()
                referenced_symbol["incoming_jumps"].add(symbol_name)
        if "outgoing_pointers" in symbol:
            for outgoing in symbol["outgoing_pointers"]:
                if outgoing not in all_symbols:
                    # print(outgoing, symbol_name)
                    continue
                # print(all_symbols[outgoing], symbol_name)

                referenced_symbol = all_symbols[outgoing]
                if "incoming_pointers" not in referenced_symbol:
                    # print(symbol_name, "->", outgoing)
                    referenced_symbol["incoming_pointers"] = set()
                referenced_symbol["incoming_pointers"].add(symbol_name)

    print(all_symbols["exception_table"])

    # Chart it all
    print("charting {} symbols".format(len(all_symbols)))
    callgraph = pgv.AGraph(directed=True)
    for i, symbol_name in enumerate(all_symbols):
        symbol = all_symbols[symbol_name]
        # print(i, symbol_name)
        # if "outgoing_jumps" in symbol:
        #     print("   ", len(symbol["outgoing_jumps"]), "jumps")
        # if "outgoing_pointers" in symbol:
        #     print("   ", len(symbol["outgoing_pointers"]), "ptrs")
        # if i > 3000:
        #     break
        if ("incoming_jumps" not in symbol or len(symbol["incoming_jumps"]) == 0) and (
            "incoming_pointers" not in symbol or len(symbol["incoming_pointers"]) == 0
        ):
            # print(symbol_name)
            continue
        if "start_address" not in symbol:
            continue
        callgraph.add_node(symbol_name)
        if "outgoing_jumps" in symbol:
            for outgoing in symbol["outgoing_jumps"]:
                callgraph.add_edge(symbol_name, outgoing)
        if "outgoing_pointers" in symbol:
            for outgoing in symbol["outgoing_pointers"]:
                callgraph.add_edge(symbol_name, outgoing, color="red")
        # print(symbol_name, symbol)

    # Style all of the nodes
    print("styling")
    for node in callgraph.iternodes():
        if node.name not in all_symbols:
            continue
        symbol = all_symbols[node.name]
        node.attr["shape"] = "box"
        text_width_ish = len(node.name) * 0.1
        if "size" not in symbol:
            print(symbol)
        size = symbol["size"] / 8
        square_size = size ** 0.5
        if text_width_ish > square_size:
            w = text_width_ish
            h = size / text_width_ish
        else:
            w = square_size
            h = square_size
        node.attr["width"] = w
        node.attr["height"] = h
        node.attr["label"] = node.name + "\r\n" + str(symbol["size"]) + " bytes"
        node.attr["style"] = "filled"

        incoming = 0
        if "incoming_jumps" in symbol:
            incoming += len(symbol["incoming_jumps"])
        if "incoming_pointers" in symbol:
            incoming += len(symbol["incoming_pointers"])

        if node.name in SPECIAL_NODE_COLORS:
            node.attr["color"] = SPECIAL_NODE_COLORS[node.name]
        elif incoming == 1:
            node.attr["color"] = "lightblue"
        elif incoming > 25:
            print("delete", node.name, "because it has {} incoming".format(incoming))
            callgraph.delete_node(node.name)
        elif incoming > 15:
            node.attr["color"] = "red"

    print("drawing")
    callgraph.layout(prog="dot")
    fn = "callgraph.svg"
    print(fn)
    callgraph.draw(fn)


if __name__ == "__main__":
    do_all_the_things()
