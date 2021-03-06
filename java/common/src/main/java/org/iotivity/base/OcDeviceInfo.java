/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.List;

/**
 * This class is expected as input for device properties. Device name and types are mandatory
 * and expected from the application. Device id of type UUID will be generated by the stack.
 */
public class OcDeviceInfo {

    private String mDeviceName;
    private List<String> mDeviceTypes;

    public OcDeviceInfo(String deviceName, List<String> deviceTypes) {
        this.mDeviceName = deviceName;
        this.mDeviceTypes = deviceTypes;
    }

    public String getDeviceName() {
        return mDeviceName;
    }

    public List<String> getDeviceTypes() {
        return mDeviceTypes;
    }
}
