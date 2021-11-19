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
import com.example.distschedule.bean.DevicesType;
import ohos.app.Context;
import ohos.data.DatabaseHelper;
import ohos.data.preferences.Preferences;

import java.util.List;
import java.util.UUID;

public class PreferencesUtil {
    private static final String KEY_USER_ID = "User_ID";
    private static final String KEY_USER_PHONE = "User_Phone";
    private static final String KEY_USER_NAME = "User_Name";
    private static final String KEY_FAMILY_ID = "Family_ID";
    private static final String KEY_FAMILY_Name = "Family_Name";
    private static final String PREFERENCE_FILE_NAME = "DistSchedule_Preferences";
    private static final String KEY_DEVICE_INFO = "Device_Info";
    private static final String KEY_DEVICE_TYPE = "Device_Type";
    private static Preferences preferences;
    private static DatabaseHelper databaseHelper;

    private static void initPreferences(Context context) {
        if (databaseHelper == null) {
            databaseHelper = new DatabaseHelper(context);
        }
        if (preferences == null) {
            preferences = databaseHelper.getPreferences(PREFERENCE_FILE_NAME);
        }
    }

    public static Preferences getPreferences(Context context) {
        initPreferences(context);
        return preferences;
    }

    public static void putUserInfo(Context context, UUID userId, String name, String phone) {
        initPreferences(context);
        preferences.putString(KEY_USER_ID, userId.toString());
        preferences.putString(KEY_USER_PHONE, phone);
        preferences.putString(KEY_USER_NAME, name);
        preferences.flush();
    }

    public static String getDeviceInfo(Context context) {
        initPreferences(context);
        return preferences.getString(KEY_DEVICE_INFO, "");
    }

    public static void putUserFamilyInfo(Context context, String familyId, String FamilyName) {
        initPreferences(context);
        preferences.putString(KEY_FAMILY_ID, familyId);
        preferences.putString(KEY_FAMILY_Name, FamilyName);
        preferences.flush();
    }

    public static String getUserFamilyName(Context context) {
        initPreferences(context);
        return preferences.getString(KEY_FAMILY_Name, "");
    }

    public static String getUserId(Context context) {
        initPreferences(context);
        return preferences.getString(KEY_USER_ID, "");
    }

    public static String getUserPhone(Context context) {
        initPreferences(context);
        return preferences.getString(KEY_USER_PHONE, "");
    }

    public static String getUserName(Context context) {
        initPreferences(context);
        return preferences.getString(KEY_USER_NAME, "");
    }

    public static void putString(Context context, String key, String value) {
        initPreferences(context);
        preferences.putString(key, value);
        preferences.flush();
    }

    public static String getString(Context context, String key) {
        initPreferences(context);
        return preferences.getString(key, null);
    }

    public static boolean deletePreferences(Context context) {
        initPreferences(context);
        return databaseHelper.deletePreferences(PREFERENCE_FILE_NAME);
    }

    public static void putDeviceType(Context context, List<DevicesType> devicesTypeList) {
        initPreferences(context);
        preferences.putString(KEY_DEVICE_TYPE, JSONArray.toJSONString(devicesTypeList));
        preferences.flush();
    }

    public static List<DevicesType> getDeviceType(Context context) {
        initPreferences(context);
        String jsonString = preferences.getString(KEY_DEVICE_TYPE, "null");
        return JSONArray.parseArray(jsonString, DevicesType.class);
    }
}
