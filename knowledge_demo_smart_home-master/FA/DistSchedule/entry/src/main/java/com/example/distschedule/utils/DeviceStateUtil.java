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

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import com.socks.library.KLog;

public class DeviceStateUtil {
    // 开关状态 ON 打开，OFF 关闭
    public static boolean getSwitchOnStatus(String data, String key) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            String SwitchOnStatus = properties.getString(key);
            return "ON".equals(SwitchOnStatus);
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return false;
        }
    }

    // 开关状态 OPEN 打开，CLOSE 关闭
    public static boolean getSwitchOpenStatus(String data, String key) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            String SwitchOpenStatus = properties.getString(key);
            return "OPEN".equals(SwitchOpenStatus);
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return false;
        }
    }

    // 获取 JSON key 值
    public static String getKeyValue(String data, String key) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            return properties.getString(key);
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return "";
        }

    }

    // 灯开关状态
    public static boolean getLampStatus(String data) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            String lampStatus = properties.getString("LampStatus");
            return "ON".equals(lampStatus);
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return false;
        }
    }

    // 灯亮度
    public static String getDutyCycle(String data) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            return properties.getString("DutyCycle");
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return "0";
        }

    }

    // 窗帘状态
    public static boolean getCurtainStatus(String data) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            String curtainStatus = properties.getString("CurtainStatus");
            return "OPEN".equals(curtainStatus);
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return false;
        }
    }

    // 浇花状态
    public static boolean getMotorStatus(String data) {
        String wateringSwitch = "";
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            wateringSwitch = properties.getString("MotorStatus");
            return "ON".equals(wateringSwitch);
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return false;
        }
    }

    // 养花当前空气的温度
    public static String getTemperature(String data) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            return properties.getString("Temperature");
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return "0";
        }
    }

    // 养花当前空气的湿度
    public static String getHumidity(String data) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            return properties.getString("Humidity");
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return "0";
        }

    }

    // 养花土壤水分
    public static String getMoisture(String data) {
        try {
            JSONObject jsonObject = JSONObject.parseObject(data);
            JSONArray services = jsonObject.getJSONArray("services");
            JSONObject item = services.getJSONObject(0);
            JSONObject properties = item.getJSONObject("properties");
            return properties.getString("Moisture");
        } catch (Exception e) {
            KLog.w(e.getMessage());
            return "0";
        }
    }
}
