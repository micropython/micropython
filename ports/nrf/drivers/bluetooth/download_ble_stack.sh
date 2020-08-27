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
    wget --post-data="fileName=DeviceDownload&ids=DBBEB2467E4A4EBCB791C2E7BE3FC7A8" https://www.nordicsemi.com/api/sitecore/Products/MedialibraryZipDownload2
    mv MedialibraryZipDownload2 temp.zip
    unzip -u temp.zip
    unzip -u s110nrf51800.zip
    rm s110nrf51800.zip
    rm temp.zip
    cd -
}

function download_s132_nrf52_7_2_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_7.2.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s132_nrf52_7.2.0
    cd $1/s132_nrf52_7.2.0
    wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SoftDevices/S132/s132nrf52720.zip
    unzip -u s132nrf52720.zip
    rm s132nrf52720.zip
    cd -
}

function download_s140_nrf52_7_2_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s140_nrf52_7.2.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s140_nrf52_7.2.0
    cd $1/s140_nrf52_7.2.0
    wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SoftDevices/S140/s140nrf52720.zip
    unzip -u s140nrf52720.zip
    rm s140nrf52720.zip
    cd -
}

SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then 
    echo "No Bluetooth LE stack defined, downloading all."
    download_s110_nrf51_8_0_0 ${SCRIPT_DIR}
    download_s132_nrf52_7_2_0 ${SCRIPT_DIR}
    download_s140_nrf52_7_2_0 ${SCRIPT_DIR}
else 
    case $1 in
        "s110_nrf51" )
            download_s110_nrf51_8_0_0 ${SCRIPT_DIR} ;;
        "s132_nrf52_7_2_0" )
            download_s132_nrf52_7_2_0 ${SCRIPT_DIR} ;;
        "s140_nrf52_7_2_0" )
            download_s140_nrf52_7_2_0 ${SCRIPT_DIR} ;;
    esac
fi

exit 0
