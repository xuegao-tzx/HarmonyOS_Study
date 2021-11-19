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

package com.example.distschedule.database;

import com.alibaba.fastjson.JSONObject;
import com.j256.ormlite.field.DatabaseField;
import com.j256.ormlite.table.DatabaseTable;

/**
 * Function description
 * AlarmBean
 */
@DatabaseTable(tableName = "Alarm")
public class AlarmBean {
    /**
     * COLUMNNAME_ID
     */
    public static final String COLUMN_ID = "id";
    public static final String COLUMN_TYPE = "type";
    public static final String COLUMN_MESSAGE = "message";
    public static final String COLUMN_DEVICE_ID = "deviceId";
    public static final String COLUMN_TIME = "time";
    public static final String COLUMN_USER_ID = "userId";

    @DatabaseField(generatedId = true, columnName = COLUMN_ID, useGetSet = true)
    private int id;
    @DatabaseField(columnName = COLUMN_TYPE, useGetSet = true, canBeNull = false)
    private int type;
    @DatabaseField(columnName = COLUMN_DEVICE_ID, useGetSet = true, canBeNull = false)
    private String deviceId;
    @DatabaseField(columnName = COLUMN_MESSAGE, useGetSet = true)
    private String message;
    @DatabaseField(columnName = COLUMN_TIME, useGetSet = true)
    private String time;
    @DatabaseField(columnName = COLUMN_USER_ID, useGetSet = true, canBeNull = false)
    private String userId;

    /**
     * constructor
     */
    public AlarmBean() {
    }

    public AlarmBean(int type, String deviceId, String message, String time, String userId) {
        this.type = type;
        this.deviceId = deviceId;
        this.message = message;
        this.time = time;
        this.userId = userId;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }

    @Override
    public String toString() {
        return JSONObject.toJSONString(this);
    }

    /**
     * 判断是否为紧急告警
     *
     * @return 是否为紧急告警
     */
    public boolean isCriticalAlarm() {
        return type == 2;
    }


    /**
     * @return 返回告警类型
     */
    public String getTypeValue() {
        if (isCriticalAlarm()) {
            return "紧急告警";
        } else {
            return "一般告警";
        }
    }
}
