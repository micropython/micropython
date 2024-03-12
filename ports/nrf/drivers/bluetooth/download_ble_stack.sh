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

function download_s140_nrf52_6_1_1
{
    echo ""
    echo "####################################"
    echo "### Downloading s140_nrf52_6.1.1 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s140_nrf52_6.1.1
    cd $1/s140_nrf52_6.1.1
    wget --post-data="fileName=DeviceDownload&ids=CE89BA7633C540AFA48AB88E934DBF05" https://www.nordicsemi.com/api/sitecore/Products/MedialibraryZipDownload2
    mv MedialibraryZipDownload2 temp.zip
    unzip -u temp.zip
    unzip -u s140nrf52611.zip
    rm s140nrf52611.zip
    rm temp.zip
    cd -
}

function download_s140_nrf52_7_3_0
{
    echo ""
    echo "####################################"
    echo "### Downloading s140_nrf52_7.3.0 ###"
    echo "####################################"
    echo ""

    mkdir -p $1/s140_nrf52_7.3.0
    cd $1/s140_nrf52_7.3.0
    wget --post-data="fileName=DeviceDownload&ids=59452FDD13BA46EEAD0810A57359F294" https://www.nordicsemi.com/api/sitecore/Products/MedialibraryZipDownload2
    mv MedialibraryZipDownload2 temp.zip
    unzip -u temp.zip
    unzip -u s140_nrf52_7.3.0.zip
    rm s140_nrf52_7.3.0.zip
    rm temp.zip
    cd -
}

SCRIPT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ $# -eq 0 ]; then
    echo "No Bluetooth LE stack defined, downloading all."
    download_s110_nrf51_8_0_0 ${SCRIPT_DIR}
    download_s132_nrf52_6_1_1 ${SCRIPT_DIR}
    download_s140_nrf52_6_1_1 ${SCRIPT_DIR}
    download_s140_nrf52_7_3_0 ${SCRIPT_DIR}
else
    case $1 in
        "s110_nrf51" )
            download_s110_nrf51_8_0_0 ${SCRIPT_DIR} ;;
        "s132_nrf52_6_1_1" )
            download_s132_nrf52_6_1_1 ${SCRIPT_DIR} ;;
        "s140_nrf52_6_1_1" )
            download_s140_nrf52_6_1_1 ${SCRIPT_DIR} ;;
        "s140_nrf52_7_3_0" )
            download_s140_nrf52_7_3_0 ${SCRIPT_DIR} ;;
    esac
fi

exit 0
