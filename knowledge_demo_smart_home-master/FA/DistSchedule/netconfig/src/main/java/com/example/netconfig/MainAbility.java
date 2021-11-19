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

package com.example.netconfig;

import com.huawei.ailifeability.NetConfigAbility;
import com.huawei.ailifeability.utils.LogUtil;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.IntentParams;
import ohos.ace.ability.AceAbility;

import java.util.Base64;

public class MainAbility extends AceAbility {
    private static String productId;
    private String productName = "FAN";

    @Override
    public void onStart(Intent intent) {
        String sessionId = intent.getStringParam("sessionId");
        if ("null".equals(sessionId) || "".equals(sessionId) || "NAN_DEVICE_NOT_FOUND".equals(sessionId)) {
            NetConfigAbility.register(this, null);
        } else {
            NetConfigAbility.register(this, sessionId);
        }

        IntentParams intentParams = intent.getParams();
        if (intentParams != null) {
            intentParams.setParam("productId", productId);
            intentParams.setParam("productName", productName);
            intentParams.setParam("sessionId", sessionId); // sessionId -> NAN_DEVICE_NOT_FOUND
            String targetDeviceId = intent.getStringParam("targetDeviceId");
            String deviceId = intent.getStringParam("deviceId");
            intentParams.setParam("targetDeviceId", targetDeviceId); // sessionId -> NAN_DEVICE_NOT_FOUND
            intentParams.setParam("deviceId", deviceId);
            setPageParams(null, intentParams);
            LogUtil.info("DistSchedule_log MainAbility", "productId = " + productId + ",productName = " + productName +
                    ",sessionId = " + sessionId + ",targetDeviceId = " + targetDeviceId + ",deviceId = " + deviceId);
        }
        super.onStart(intent);
    }

    @Override
    public void onStop() {
        NetConfigAbility.deregister();
        super.onStop();
    }

    private String base64Decoder(String data91) {
        Base64.Decoder decoder = Base64.getDecoder();
        byte[] bytes = decoder.decode(data91);
        StringBuilder stringBuilder = new StringBuilder();
        for (byte byteData : bytes) {
            stringBuilder.append((char) byteData);
        }
        return stringBuilder.toString();
    }
}
