#!/usr/bin/python
import re

"""
Creates a dot file describing the state machine in aws_defender.c
"""

src_file = "aws_defender.c"
dot_file = "aws_defender_states.dot"

with open(dot_file, "w") as out_file:

    out_file.write(
        """digraph G {
        concentrate = true;
        overlap     = false;
        splines     = true;
        """)

    function_name = ""
    for line in open(src_file, "r"):
        if re.search("DEFENDER_States\[.*\].*\=.DEFENDER", line):
            state_func = line
            state_func = re.sub("DEFENDER_States\[", "", state_func)
            state_func = re.sub("\]", "", state_func)
            state_func = re.sub("\= ", "-> {", state_func)
            state_func = re.sub(";", " [shape=rectangle]}", state_func)
            if re.search("FAIL",state_func):
                state_func += "[color=red]"
            out_file.write(state_func)

        if re.search("static DefenderState_t DEFENDER_State", line):
            if not re.search(";", line):
                function_name = line
                function_name = re.sub("static DefenderState_t ", "", function_name)
                function_name = re.sub("\(.*\)", "", function_name)
                function_name = function_name.strip()

        if re.search("return DEFENDER_STATE", line):
            next_state = line
            next_state = re.sub("return|\;", "",next_state)
            next_state = next_state.strip()

            if re.search("FAIL",next_state):
                next_state += "[color=red]"

            out_file.write(function_name + " -> " + next_state + "\n")

    out_file.write("\n}\n\n")

# dot -Tpng aws_defender_states.dot -o aws_defender_states.png
