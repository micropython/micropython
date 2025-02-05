#!/usr/bin/bash

# This script will be use to set the dependencies version in the bsps deps/ folder.

# Why we need this script? 
# --------------------------------
# The ModusTooolbox sets a fix version for the bsp repo sources, but the version of the dependencies
# are not fixed. They are handled by the manifest system instead.
# As we are not keeping these bsps under version control, but we retrieve them when initializing
# the project, we will always get the latest version of the bsp dependencies (according to the manifest configuration).
# We don´t want libraries to be updated without our knowledge, so we will set the dependencies version with this script.

# Move to the directory where the script is located
cd "$(dirname "$0")"

board=$1
# Pass "-d" as the second argument to enable debug mode
debug=$2 

json_file_default_name="bsp-deps.json"
json_file="../boards/$board/$json_file_default_name"
bsp_deps_dir="bsps/TARGET_APP_$board/deps"

if [ ! -f "$json_file" ]; then
    echo "JSON file not found: $json_file"
    exit 1
fi

# Read the JSON file line by line
while IFS= read -r line; do
    # Extract the asset-name
    if [[ $line =~ \"asset-name\":\ \"([^\"]+)\" ]]; then
        asset_name="${BASH_REMATCH[1]}"
    fi

    # Extract the locked-commit
    if [[ $line =~ \"locked-commit\":\ \"([^\"]+)\" ]]; then
        locked_commit="${BASH_REMATCH[1]}"
        
        # Print the extracted values if -d flag is passed
        if [ "$debug" == "-d" ]; then
            echo "Asset Name: $asset_name"
            echo "Locked Commit: $locked_commit"
            echo "-------------------------"
        fi

        # Iterate over each file in the BSP_DEPS_DIR directory
        for file in "$bsp_deps_dir"/*; do
            if [ -f "$file" ]; then
                # Get the file name without the extension
                filename=$(basename -- "$file")
                filename="${filename%.*}"

                # Check if the file name matches the asset name
                if [ "$filename" == "$asset_name" ]; then

                    if [ "$debug" == "-d" ]; then
                        echo "Match found: $file matches asset name $asset_name"
                    fi
                    
                    # Read the content of the .mtbx file
                    content=$(cat "$file")
                    
                    # Replace the version pattern with the locked-commit value
                    updated_content=$(echo "$content" | sed -E "s/release-v[0-9]+\.[0-9]+\.[0-9]+/$locked_commit/g")
                    
                    # Write the updated content back to the .mtbx file
                    echo "$updated_content" > "$file"
                    
                    echo "Updated $file with locked commit $locked_commit"
                else
                    if [ "$debug" == "-d" ]; then
                        echo "No match: $file does not match asset name $asset_name"
                    fi
                fi
            fi
        done
    fi
done < "$json_file"
 
exit 0