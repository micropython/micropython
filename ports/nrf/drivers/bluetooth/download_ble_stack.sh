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


function download_s132_nrf52_6_0_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_6.0.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s132_nrf52_6.0.0
    cd $1/s132_nrf52_6.0.0

    wget http://www.nordicsemi.com/eng/nordic/download_resource/67248/3/62916494/141008
    mv 141008 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}


function download_s140_nrf52_6_0_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s140_nrf52_6.0.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s140_nrf52_6.0.0
    cd $1/s140_nrf52_6.0.0

    wget http://www.nordicsemi.com/eng/nordic/download_resource/60624/19/81980817/116072
    mv 116072 temp.zip
    unzip -u temp.zip
    rm temp.zip
    cd -
}


SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then 
    echo "No Bluetooth LE stack defined, downloading all."
    download_s110_nrf51_8_0_0 ${SCRIPT_DIR}
    download_s132_nrf52_6_0_0 ${SCRIPT_DIR}
    download_s140_nrf52_6_0_0 ${SCRIPT_DIR}
else 
    case $1 in
        "s110_nrf51" )
            download_s110_nrf51_8_0_0 ${SCRIPT_DIR} ;;
        "s132_nrf52_2_0_1" )
            download_s132_nrf52_6_0_0 ${SCRIPT_DIR} ;;
        "s132_nrf52_3_0_0" )
            download_s140_nrf52_6_0_0 ${SCRIPT_DIR} ;;
    esac
fi

exit 0
