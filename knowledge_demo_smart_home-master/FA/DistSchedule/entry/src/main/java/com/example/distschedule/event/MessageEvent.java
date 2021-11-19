/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

package com.example.distschedule.event;

import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.bean.Command;

public class MessageEvent {
    public static final String EVENT_GET_COMMAND = "Event_Get_Command";
    public static final String EVENT_GET_DEVICE_JSON = "Event_Get_Device_Json";
    public static final String EVENT_GET_MEMBER = "Event_Get_Member";
    public static final String EVENT_GET_SHARE_MEMBER = "Event_Get_Share_Member";
    private String key;
    private Command command;
    private String deviceId;
    private String message;
    private JSONObject jsonObject;

    public MessageEvent(String key, String deviceId, Command command) {
        this.key = key;
        this.deviceId = deviceId;
        this.command = command;
    }

    public MessageEvent(String key, JSONObject jsonObject) {
        this.key = key;
        this.jsonObject = jsonObject;
    }

    public MessageEvent(String key, String message) {
        this.key = key;
        this.message = message;
    }

    public String getMessage() {
        return message;
    }

    public JSONObject getJsonObject() {
        return jsonObject;
    }

    public void setJsonObject(JSONObject jsonObject) {
        this.jsonObject = jsonObject;
    }

    public String getKey() {
        return key;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public Command getCommand() {
        return command;
    }

    public void setCommand(Command command) {
        this.command = command;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }
}
