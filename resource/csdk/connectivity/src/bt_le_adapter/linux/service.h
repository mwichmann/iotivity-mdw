/******************************************************************
 *
 * Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef CA_BLE_LINUX_SERVICE_H
#define CA_BLE_LINUX_SERVICE_H

#include "bluez-glue.h"
#include "object_manager-glue.h"
#include "characteristic.h"


/**
 * GATT Service Information
 */
typedef struct CAGattService
{
    /// D-Bus object path for the GattService1 object.
    char * object_path;

    /**
     * OIC GATT service object_manager D-Bus interface skeleton
     * object.
     */
    ObjectManager * object_manager;

    /// OIC GATT service D-Bus interface skeleton object.
    GattService1 * service;

    /// OIC GATT request characteristic information.
    CAGattCharacteristic request_characteristic;

    /// OIC GATT response characteristic information.
    CAGattCharacteristic response_characteristic;

    /**
     * org.bluez.GattManager1 object with which the service is
     * registered.
     */
    GDBusProxy * gatt_manager;

} CAGattService;

/**
 * Make the pseudo-address for the peer connected to the service.
 *
 * @param[in] c Information about GATT service for which the
 *              peer (client) address is being created.
 *
 * @return @c String containing an encoded address associated with the
 *         peer connected to the peripheral on which the service
 *         implementation resides, or @c NULL on error.
 */
char * CAGattServiceMakePeerAddress(CAGattService * s);

/**
 * Decode @c CAGattService pointer from peer @c address.
 *
 * @param[in] address String containing a client/peer pseudo-address
 *                    corresponding to the GATT service through which
 *                    communication is performed.
 *
 * @return Pointer to ::CAGattService object corresponding to the
 *         given address.
 */
CAGattService * CAGattServiceDecodeAddress(char const * address);

/**
 * Initialize GATT service fields.
 *
 * This function initializes the @c CAGattService object fields.
 *
 * @param[out] service  GATT service information to be initialized.
 * @param[in]  hci_name Name of the bluetooth adapter installed on the
 *                      system, e.g. @c "hci0".
 * @param[in]  context  Object containing the D-Bus connection to the
 *                      bus on which the service will be exported, as
 *                      well as an address-to-characteristic map.
 *
 * @return @c true on success, @c false otherwise.
 *
 * @note This function does not allocate the @a service object
 *       itself.  The caller is responsible for allocating that
 *       memory.
 */
bool CAGattServiceInitialize(CAGattService * service,
                             char const * hci_name,
                             CALEContext * context);

/**
 * Destroy GATT service fields.
 *
 * This function finalizes the @c CAGattService object fields.
 *
 * @param[in] service GATT service information to be finalized.
 *
 * @note This function does not deallocate the @a service object
 *       itself.  The caller is responsible for deallocating that
 *       memory.
 */
void CAGattServiceDestroy(CAGattService * service);

#endif  // CA_BLE_LINUX_SERVICE_H
