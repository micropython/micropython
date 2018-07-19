/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_MODULE_BLEIO_ADVERTISEMENTDATA_H
#define MICROPY_INCLUDED_SHARED_MODULE_BLEIO_ADVERTISEMENTDATA_H

#include "py/obj.h"

// Taken from https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
enum {
    AdFlags = 0x01,
    AdIncompleteListOf16BitServiceClassUUIDs = 0x02,
    AdCompleteListOf16BitServiceClassUUIDs = 0x03,
    AdIncompleteListOf32BitServiceClassUUIDs = 0x04,
    AdCompleteListOf32BitServiceClassUUIDs = 0x05,
    AdIncompleteListOf128BitServiceClassUUIDs = 0x06,
    AdCompleteListOf128BitServiceClassUUIDs = 0x07,
    AdShortenedLocalName = 0x08,
    AdCompleteLocalName = 0x09,
    AdTxPowerLevel = 0x0A,
    AdClassOfDevice = 0x0D,
    AdSimplePairingHashC = 0x0E,
    AdSimplePairingRandomizerR = 0x0F,
    AdSecurityManagerTKValue = 0x10,
    AdSecurityManagerOOBFlags = 0x11,
    AdSlaveConnectionIntervalRange = 0x12,
    AdListOf16BitServiceSolicitationUUIDs = 0x14,
    AdListOf128BitServiceSolicitationUUIDs = 0x15,
    AdServiceData = 0x16,
    AdPublicTargetAddress = 0x17,
    AdRandomTargetAddress = 0x18,
    AdAppearance = 0x19,
    AdAdvertisingInterval = 0x1A,
    AdLEBluetoothDeviceAddress = 0x1B,
    AdLERole = 0x1C,
    AdSimplePairingHashC256 = 0x1D,
    AdSimplePairingRandomizerR256 = 0x1E,
    AdListOf32BitServiceSolicitationUUIDs = 0x1F,
    AdServiceData32BitUUID = 0x20,
    AdServiceData128BitUUID = 0x21,
    AdLESecureConnectionsConfirmationValue = 0x22,
    AdLESecureConnectionsRandomValue = 0x23,
    AdURI = 0x24,
    AdIndoorPositioning = 0x25,
    AdTransportDiscoveryData = 0x26,
    AdLESupportedFeatures = 0x27,
    AdChannelMapUpdateIndication = 0x28,
    AdPBADV = 0x29,
    AdMeshMessage = 0x2A,
    AdMeshBeacon = 0x2B,
    Ad3DInformationData = 0x3D,
    AdManufacturerSpecificData = 0xFF,
};

typedef struct {
    mp_obj_t device_name;
    mp_obj_t services;
    mp_obj_t data;
    bool connectable;
} bleio_advertisement_data_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_BLEIO_ADVERTISEMENTDATA_H
