#!/bin/bash

if [ "$#" != 3 ]; then
    echo "Usage: add-memzip.sh input.hex output.hex file-directory"
    exit 1
fi

#set -x

input_hex=$1
output_hex=$2
memzip_src_dir=$3

input_bin=${input_hex}.bin
output_bin=${output_hex}.bin
zip_file=${output_hex}.zip
zip_base=$(basename ${zip_file})
zip_dir=$(dirname ${zip_file})
abs_zip_dir=$(realpath ${zip_dir})

rm -f ${zip_file}
(cd ${memzip_src_dir}; zip -0 -r -D ${abs_zip_dir}/${zip_base} .)
objcopy -I ihex -O binary ${input_hex} ${input_bin}
cat ${input_bin} ${zip_file} > ${output_bin}
objcopy -I binary -O ihex ${output_bin} ${output_hex}
echo "Added ${memzip_src_dir} to ${input_hex} creating ${output_hex}"

