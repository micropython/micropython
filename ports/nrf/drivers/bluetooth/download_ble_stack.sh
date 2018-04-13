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
    wget https://www.nordicsemi.com/eng/nordic/download_resource/51479/6/84640562/95151
    mv 95151 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}

function download_s132_nrf52_5_0_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_5.0.0 ###"
    echo "####################################"
    echo ""

    mkdir -p "${1}/s132_nrf52_5.0.0"
    cd "${1}/s132_nrf52_5.0.0"

    wget http://www.nordicsemi.com/eng/nordic/download_resource/58987/11/7198220/116068
    mv 116068 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}

function download_s140_nrf52_6_0_0_6_alpha
{
    echo ""
    echo "############################################"
    echo "### Downloading s140_nrf52_6.0.0-6.alpha ###"
    echo "############################################"
    echo ""

    mkdir -p "${1}/s140_nrf52_6.0.0-6.alpha"
    cd "${1}/s140_nrf52_6.0.0-6.alpha"

    wget http://www.nordicsemi.com/eng/nordic/download_resource/60624/18/61756799/116072
    mv 116072 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}

SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then
    echo "No Bluetooth LE stack defined, downloading all."
    download_s132_nrf52_2_0_1 "${SCRIPT_DIR}"
    download_s132_nrf52_5_0_0 "${SCRIPT_DIR}"
    download_s140_nrf52_6_0_0_6_alpha "${SCRIPT_DIR}"
else
    case $1 in
        "s132_nrf52_2_0_1" )
            download_s132_nrf52_2_0_1 "${SCRIPT_DIR}" ;;
        "s132_nrf52_5_0_0" )
            download_s132_nrf52_5_0_0 "${SCRIPT_DIR}" ;;
        "s140_nrf52_6_0_0_6_alpha" )
            download_s140_nrf52_6_0_0_6_alpha "${SCRIPT_DIR}" ;;
    esac
fi

exit 0
