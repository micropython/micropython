#!/bin/bash

function download_s110_nrf51_8_0_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s110_nrf51_8.0.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s110_nrf51_8.0.0
    cd $1/s110_nrf51_8.0.0
    wget https://www.nordicsemi.com/eng/nordic/download_resource/45846/3/78153065/80234
    mv 80234 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}

function download_s132_nrf52_2_0_1
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_2.0.1 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s132_nrf52_2.0.1
    cd $1/s132_nrf52_2.0.1
    wget https://www.nordicsemi.com/eng/nordic/download_resource/51479/6/84640562/95151
    mv 95151 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}

function download_s132_nrf52_3_0_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_3.0.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s132_nrf52_3.0.0
    cd $1/s132_nrf52_3.0.0

    wget https://www.nordicsemi.com/eng/nordic/download_resource/56261/6/26298825/108144
    mv 108144 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}


SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then 
    echo "No Bluetooth LE stack defined, downloading all."
    download_s110_nrf51_8_0_0 ${SCRIPT_DIR}
    download_s132_nrf52_2_0_1 ${SCRIPT_DIR}
    download_s132_nrf52_3_0_0 ${SCRIPT_DIR}
else 
    case $1 in
        "s110_nrf51" )
            download_s110_nrf51_8_0_0 ${SCRIPT_DIR} ;;
        "s132_nrf52_2_0_1" )
            download_s132_nrf52_2_0_1 ${SCRIPT_DIR} ;;
        "s132_nrf52_3_0_0" )
            download_s132_nrf52_3_0_0 ${SCRIPT_DIR} ;;
    esac
fi

exit 0
