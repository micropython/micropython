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

function download_s132_nrf52_6_1_1
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_6.1.1 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s132_nrf52_6.1.1
    cd $1/s132_nrf52_6.1.1
    wget --post-data="fileName=DeviceDownload&ids=3AB3E86666FE4361A4A3B7E0D1CBB9B9" https://www.nordicsemi.com/api/sitecore/Products/MedialibraryZipDownload2
    mv MedialibraryZipDownload2 temp.zip
    unzip -u temp.zip
    unzip -u s132nrf52611.zip
    rm s132nrf52611.zip
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
    wget --post-data="fileName=DeviceDownload&ids=D631FCC10C9741A49135BC0450E42B19" https://www.nordicsemi.com/api/sitecore/Products/MedialibraryZipDownload2
    mv MedialibraryZipDownload2 temp.zip
    unzip -u temp.zip
    unzip -u s140nrf52600.zip
    rm s140nrf52600.zip
    rm temp.zip
    cd -
}


SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then 
    echo "No Bluetooth LE stack defined, downloading all."
    download_s110_nrf51_8_0_0 ${SCRIPT_DIR}
    download_s132_nrf52_6_1_1 ${SCRIPT_DIR}
    download_s140_nrf52_6_0_0 ${SCRIPT_DIR}
else 
    case $1 in
        "s110_nrf51" )
            download_s110_nrf51_8_0_0 ${SCRIPT_DIR} ;;
        "s132_nrf52_6_1_1" )
            download_s132_nrf52_6_1_1 ${SCRIPT_DIR} ;;
        "s140_nrf52_6_0_0" )
            download_s140_nrf52_6_0_0 ${SCRIPT_DIR} ;;
    esac
fi

exit 0
