/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.distschedule.utils;

import ohos.agp.window.service.Display;
import ohos.agp.window.service.DisplayManager;
import ohos.app.Context;
import ohos.data.distributed.common.KvManagerConfig;
import ohos.data.distributed.common.KvManagerFactory;
import ohos.distributedhardware.devicemanager.DeviceInfo;

import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Optional;

public class DeviceInfoUtil {
    public static DeviceInfo.DeviceType getDeviceType(Context context) {
        DisplayManager displayManager = DisplayManager.getInstance();
        Optional<Display> defaultDisplay = displayManager.getDefaultDisplay(context);
        double diagonalInches = 0;
        if (defaultDisplay.isPresent()) {
            double yInches = defaultDisplay.get().getAttributes().height / defaultDisplay.get().getAttributes().yDpi;
            double xInches = defaultDisplay.get().getAttributes().width / defaultDisplay.get().getAttributes().xDpi;
            diagonalInches = Math.sqrt(xInches * xInches + yInches * yInches);
        }
        if (diagonalInches > 10.1) {
            return DeviceInfo.DeviceType.TV;
        } else if (diagonalInches <= 10.1 && diagonalInches > 7) {
            return DeviceInfo.DeviceType.TABLET;
        } else if (diagonalInches <= 7 && diagonalInches > 6.5) {
            return DeviceInfo.DeviceType.UNKNOWN_TYPE;
        } else if (diagonalInches <= 6.5 && diagonalInches >= 2) {
            return DeviceInfo.DeviceType.PHONE;
        } else {
            return DeviceInfo.DeviceType.WEARABLE;
        }
    }

    public static String getDeviceId(Context context) {
        return KvManagerFactory.getInstance().createKvManager(new KvManagerConfig(context)).getLocalDeviceInfo().getId();
    }

    public static String getMacAddress(Context context) throws SocketException {
        NetworkInterface networkInterface = NetworkInterface.getByName("wlan1");

        byte[] mac = networkInterface.getHardwareAddress();
        if (mac == null) return null;
        StringBuilder sb = new StringBuilder(18);
        for (byte b : mac) {
            if (sb.length() > 0) sb.append(':');
            sb.append(String.format("%02x", b));
        }
        return sb.toString();
    }
}
