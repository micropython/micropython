#!/bin/bash

# Use this script to setup your development environment.
# With the ModusToolbox installed:
#
# $ source dev-setup.sh && toolchain_setup [path_to_modustoolbox]
# 
# The path to the ModusToolbox is required if installed somewhere
# else than in the default home path (~/ModusToolbox)

function set_mtb_tools_path {
    mtb_path=$1
    if [ -z "$mtb_path" ]; then
        mtb_path=/opt/Tools/ModusToolbox
    fi
    
    echo ${mtb_path}/tools_3.6
}

function set_gcc_path {
    tools_path=$2
    if [ -z "$tools_path" ]; then
        tools_path=/opt/Tools
    fi
    
    echo ${tools_path}/mtb-gcc-arm-eabi/14.2.1
}

function export_path {
    mtb_tools_path=$(set_mtb_tools_path "$1")
    gcc_path=$(set_gcc_path "$1" "$2")
    export PATH=${mtb_tools_path}/library-manager:${gcc_path}/gcc/bin:$PATH
}

function toolchain_setup {
    mtb_path=$1
    export_path ${mtb_path}
}
