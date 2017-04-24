#!/bin/bash

function download_s110_nrf51
{
    echo ""
    echo "####################################"
    echo "### Downloading s110_nrf51_8.0.0 ###"
    echo "####################################"
    echo ""

    mkdir s110_nrf51_8.0.0
    cd s110_nrf51_8.0.0
    wget https://www.nordicsemi.com/eng/nordic/download_resource/45846/3/78153065/80234
    mv 80234 temp.zip
    unzip temp.zip
    rm temp.zip
    cd -
}

function download_s132_nrf52
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_3.0.0 ###"
    echo "####################################"
    echo ""

    mkdir s132_nrf52_3.0.0
    cd s132_nrf52_3.0.0

    wget https://www.nordicsemi.com/eng/nordic/download_resource/56261/6/26298825/108144
    mv 108144 temp.zip
    unzip temp.zip
    rm temp.zip
    cd -
}

if [ $# -eq 0 ]; then 
    echo "No Bluetooth LE stack defined, downloading all."
    download_s110_nrf51
    download_s132_nrf52
else 
    case $1 in
        "s110_nrf51" )
            download_s110_nrf51 ;;
        "s132_nrf52" )
            download_s132_nrf52 ;;
    esac
fi

exit 0
