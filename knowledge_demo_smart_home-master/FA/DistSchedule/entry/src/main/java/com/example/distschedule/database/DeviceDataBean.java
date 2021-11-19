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

package com.example.distschedule.database;

import com.alibaba.fastjson.JSONObject;

import java.util.function.Function;

interface parse {
    String parseCommand();
}

public class DeviceDataBean {
    private String action;
    private int offlineImage;
    private int onlineImage;
    private Function<JSONObject, String> function;

    public DeviceDataBean(String action, int onlineImage, int offlineImage, Function<JSONObject, String> function) {
        this.action = action;
        this.offlineImage = offlineImage;
        this.onlineImage = onlineImage;
        this.function = function;
    }

    public String getCommandString(JSONObject command) {
        if (function != null) {
            return function.apply(command);
        } else {
            return "";
        }
    }

    public String getAction() {
        return action;
    }

    public int getOfflineImage() {
        return offlineImage;
    }

    public int getOnlineImage() {
        return onlineImage;
    }

}
