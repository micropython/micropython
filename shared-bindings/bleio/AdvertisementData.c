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

#include "py/obj.h"
#include "shared-module/bleio/AdvertisementData.h"

//| .. currentmodule:: bleio
//|
//| :class:`AdvertisementData` -- data used during BLE advertising
//| ==============================================================
//|
//| Represents the data to be broadcast during BLE advertising.
//|

STATIC const mp_rom_map_elem_t bleio_advertisementdata_locals_dict_table[] = {
    // Static variables
    { MP_ROM_QSTR(MP_QSTR_FLAGS),                                         MP_ROM_INT(AdFlags) },
    { MP_ROM_QSTR(MP_QSTR_INCOMPLETE_LIST_OF_16BIT_SERVICE_CLASS_UUIDS),  MP_ROM_INT(AdIncompleteListOf16BitServiceClassUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_COMPLETE_LIST_OF_16BIT_SERVICE_CLASS_UUIDS),    MP_ROM_INT(AdCompleteListOf16BitServiceClassUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_INCOMPLETE_LIST_OF_32BIT_SERVICE_CLASS_UUIDS),  MP_ROM_INT(AdIncompleteListOf32BitServiceClassUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_COMPLETE_LIST_OF_32BIT_SERVICE_CLASS_UUIDS),    MP_ROM_INT(AdCompleteListOf32BitServiceClassUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_INCOMPLETE_LIST_OF_128BIT_SERVICE_CLASS_UUIDS), MP_ROM_INT(AdIncompleteListOf128BitServiceClassUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_COMPLETE_LIST_OF_128BIT_SERVICE_CLASS_UUIDS),   MP_ROM_INT(AdCompleteListOf128BitServiceClassUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_SHORTENED_LOCAL_NAME),                          MP_ROM_INT(AdShortenedLocalName) },
    { MP_ROM_QSTR(MP_QSTR_COMPLETE_LOCAL_NAME),                           MP_ROM_INT(AdCompleteLocalName) },
    { MP_ROM_QSTR(MP_QSTR_TX_POWER_LEVEL),                                MP_ROM_INT(AdTxPowerLevel) },
    { MP_ROM_QSTR(MP_QSTR_CLASS_OF_DEVICE),                               MP_ROM_INT(AdClassOfDevice) },
    { MP_ROM_QSTR(MP_QSTR_SIMPLE_PAIRING_HASH_C),                         MP_ROM_INT(AdSimplePairingHashC) },
    { MP_ROM_QSTR(MP_QSTR_SIMPLE_PAIRING_RANDOMIZER_R),                   MP_ROM_INT(AdSimplePairingRandomizerR) },
    { MP_ROM_QSTR(MP_QSTR_SECURITY_MANAGER_TK_VALUE),                     MP_ROM_INT(AdSecurityManagerTKValue) },
    { MP_ROM_QSTR(MP_QSTR_SECURITY_MANAGER_OOB_FLAGS),                    MP_ROM_INT(AdSecurityManagerOOBFlags) },
    { MP_ROM_QSTR(MP_QSTR_SLAVE_CONNECTION_INTERVAL_RANGE),               MP_ROM_INT(AdSlaveConnectionIntervalRange) },
    { MP_ROM_QSTR(MP_QSTR_LIST_OF_16BIT_SERVICE_SOLICITATION_UUIDS),      MP_ROM_INT(AdListOf16BitServiceSolicitationUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_LIST_OF_128BIT_SERVICE_SOLICITATION_UUIDS),     MP_ROM_INT(AdListOf128BitServiceSolicitationUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_SERVICE_DATA),                                  MP_ROM_INT(AdServiceData) },
    { MP_ROM_QSTR(MP_QSTR_PUBLIC_TARGET_ADDRESS),                         MP_ROM_INT(AdPublicTargetAddress) },
    { MP_ROM_QSTR(MP_QSTR_RANDOM_TARGET_ADDRESS),                         MP_ROM_INT(AdRandomTargetAddress) },
    { MP_ROM_QSTR(MP_QSTR_APPEARANCE),                                    MP_ROM_INT(AdAppearance) },
    { MP_ROM_QSTR(MP_QSTR_ADVERTISING_INTERNAL),                          MP_ROM_INT(AdAdvertisingInterval) },
    { MP_ROM_QSTR(MP_QSTR_LE_BLUETOOTH_DEVICE_ADDRESS),                   MP_ROM_INT(AdLEBluetoothDeviceAddress) },
    { MP_ROM_QSTR(MP_QSTR_LE_ROLE),                                       MP_ROM_INT(AdLERole) },
    { MP_ROM_QSTR(MP_QSTR_SIMPLE_PAIRING_HASH_C256),                      MP_ROM_INT(AdSimplePairingHashC256) },
    { MP_ROM_QSTR(MP_QSTR_SIMPLE_PAIRING_RANDOMIZER_R256),                MP_ROM_INT(AdSimplePairingRandomizerR256) },
    { MP_ROM_QSTR(MP_QSTR_LIST_OF_32BIT_SERVICE_SOLICITATION_UUIDS),      MP_ROM_INT(AdListOf32BitServiceSolicitationUUIDs) },
    { MP_ROM_QSTR(MP_QSTR_SERVICE_DATA_32BIT_UUID),                       MP_ROM_INT(AdServiceData32BitUUID) },
    { MP_ROM_QSTR(MP_QSTR_SERVICE_DATA_128BIT_UUID),                      MP_ROM_INT(AdServiceData128BitUUID) },
    { MP_ROM_QSTR(MP_QSTR_LE_SECURE_CONNECTIONS_CONFIRMATION_VALUE),      MP_ROM_INT(AdLESecureConnectionsConfirmationValue) },
    { MP_ROM_QSTR(MP_QSTR_LE_SECURE_CONNECTIONS_RANDOM_VALUE),            MP_ROM_INT(AdLESecureConnectionsRandomValue) },
    { MP_ROM_QSTR(MP_QSTR_URI),                                           MP_ROM_INT(AdURI) },
    { MP_ROM_QSTR(MP_QSTR_INDOOR_POSITIONING),                            MP_ROM_INT(AdIndoorPositioning) },
    { MP_ROM_QSTR(MP_QSTR_TRANSPORT_DISCOVERY_DATA),                      MP_ROM_INT(AdTransportDiscoveryData) },
    { MP_ROM_QSTR(MP_QSTR_LE_SUPPORTED_FEATURES),                         MP_ROM_INT(AdLESupportedFeatures) },
    { MP_ROM_QSTR(MP_QSTR_CHANNEL_MAP_UPDATE_INDICATION),                 MP_ROM_INT(AdChannelMapUpdateIndication) },
    { MP_ROM_QSTR(MP_QSTR_PB_ADV),                                        MP_ROM_INT(AdPBADV) },
    { MP_ROM_QSTR(MP_QSTR_MESH_MESSAGE),                                  MP_ROM_INT(AdMeshMessage) },
    { MP_ROM_QSTR(MP_QSTR_MESH_BEACON),                                   MP_ROM_INT(AdMeshBeacon) },
    { MP_ROM_QSTR(MP_QSTR_3D_INFORMATION_DATA),                           MP_ROM_INT(Ad3DInformationData) },
    { MP_ROM_QSTR(MP_QSTR_MANUFACTURER_SPECIFIC_DATA),                    MP_ROM_INT(AdManufacturerSpecificData) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_advertisementdata_locals_dict, bleio_advertisementdata_locals_dict_table);

const mp_obj_type_t bleio_advertisementdata_type = {
    { &mp_type_type },
    .name = MP_QSTR_AdvertisementData,
    .locals_dict = (mp_obj_dict_t*)&bleio_advertisementdata_locals_dict
};
