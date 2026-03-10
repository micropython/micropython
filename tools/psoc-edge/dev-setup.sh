#!/bin/bash

# Use this script to setup your development environment.
# With the required tools installed:
#
# $ source dev-setup.sh && toolchain_setup

SEARCH_PATHS=(
    /opt/Tools
    /opt
    /usr/local
)

REQUIRED_TOOLS=(
    edgeprotecttools
    arm-none-eabi-gcc
    openocd
)

function find_tool_path {
    local tool=$1
    for search_path in "${SEARCH_PATHS[@]}"; do
        local result
        result=$(find "$search_path" -maxdepth 5 -type f -name "${tool}" 2>/dev/null | head -n 1)
        if [ -n "$result" ]; then
            echo "$(dirname "$result")"
            return 0
        fi
    done
    return 1
}

function is_tool_in_path {
    local tool=$1
    local result=$(which "${tool}")
    if [ -z "$result" ]; then
        return 1
    fi

    return 0
}

function toolchain_setup {
    for tool in "${REQUIRED_TOOLS[@]}"; do
        if ! is_tool_in_path "$tool"; then
            tool_path=$(find_tool_path "$tool")
            if [ -n "$tool_path" ]; then
                export PATH="$tool_path:$PATH"
                printf "%-25s: %s\n" "$tool" "$tool_path"
            else
                echo "Error: $tool not found in any of the search paths. Please install it or add it to PATH."
            fi
        else
            printf "%-25s: %s\n" "$tool" "$(which "${tool}")"
        fi

    done
}