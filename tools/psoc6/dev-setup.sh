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
    if [ "$mtb_path" = "" ]; then
        mtb_path=~/ModusToolbox
    fi
    
    echo ${mtb_path}/tools_*
}

function export_path {
    mtb_tools_path=$(set_mtb_tools_path)
    export PATH=${mtb_tools_path}/openocd/bin:${mtb_tools_path}/library-manager:${mtb_tools_path}/fw-loader/bin/:${mtb_tools_path}/gcc/bin:$PATH
}

function install_udev_rules {
    mtb_tools_path=$(set_mtb_tools_path)
    ${mtb_tools_path}/openocd/udev_rules/install_rules.sh
}

function toolchain_setup {
    mtb_path=$1
    export_path ${mtb_path}
    install_udev_rules
}

function git_add_ssh {
    ssh_key=$1
    eval $(ssh-agent -s)
    ssh-add ${ssh_key}
}
