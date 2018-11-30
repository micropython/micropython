#!/bin/bash

function download_s132_nrf52_2_0_1
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_2.0.1 ###"
    echo "####################################"
    echo ""

    mkdir -p "${1}/s132_nrf52_2.0.1"
    cd "${1}/s132_nrf52_2.0.1"
    wget https://www.nordicsemi.com/api/sitecore/Products/MedialibraryZipDownload2 --post-data="ids=863031714A574444AADFE444EBE5BA9B|&fileName=DeviceDownload" -O temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}

SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then
    echo "No Bluetooth LE stack defined, downloading all."
    download_s132_nrf52_2_0_1 "${SCRIPT_DIR}"
else
    case $1 in
        "s132_nrf52_2_0_1" )
            download_s132_nrf52_2_0_1 "${SCRIPT_DIR}" ;;
    esac
fi

exit 0
