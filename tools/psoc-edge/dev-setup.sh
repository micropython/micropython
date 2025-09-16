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
        mtb_path=~/ModusToolbox
    fi
    
    echo ${mtb_path}/tools_3.6
}

function export_path {
    mtb_tools_path=$(set_mtb_tools_path "$1")
    export PATH=${mtb_tools_path}/library-manager:$PATH
}

function toolchain_setup {
    mtb_path=$1
    export_path ${mtb_path}
}

function git_add_ssh {
    ssh_key=$1
    eval $(ssh-agent -s)
    ssh-add ${ssh_key}
}