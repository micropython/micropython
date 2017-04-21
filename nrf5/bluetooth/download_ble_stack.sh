#!/bin/bash

function download_s110_nrf51
{
    echo ""
    echo "####################################"
    echo "### Downloading s110_nrf51_8.0.0 ###"
    echo "####################################"
    echo ""

    wget https://developer.nordicsemi.com/nRF5_SDK/nRF51_SDK_v10.x.x/nRF51_SDK_10.0.0_dc26b5e.zip
    unzip nRF51_SDK_10.0.0_dc26b5e.zip components/softdevice/s110/*
    mv components s110_nrf51_8.0.0
    rm nRF51_SDK_10.0.0_dc26b5e.zip
}

function download_s132_nrf52
{
    echo ""
    echo "####################################"
    echo "### Downloading s132_nrf52_3.0.0 ###"
    echo "####################################"
    echo ""

    wget https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v12.x.x/nRF5_SDK_12.1.0_0d23e2a.zip
    unzip nRF5_SDK_12.1.0_0d23e2a.zip components/softdevice/s132/*
    mv components s132_nrf52_3.0.0
    rm nRF5_SDK_12.1.0_0d23e2a.zip
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
