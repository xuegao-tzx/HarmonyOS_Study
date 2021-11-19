/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
package com.example.distschedule.error;

import java.util.Collections;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public enum ErrorCode {
    /**
     * 通用错误码.
     */
    ILLEGAL_PARAM(Level.SERVER, Module.COMMON, "00", "illegal parameter"),
    ILLEGAL_STATE(Level.SERVER, Module.COMMON, "01", "illegal state"),
    UNSUPPORTED_OPERATION(Level.SERVER, Module.COMMON, "02", "unsupported operation"),
    ILLEGAL_PERMISSION(Level.SERVER, Module.COMMON, "03", "illegal permission"),

    /**
     * 用户错误码.
     */
    USER_NOT_FOUND(Level.SERVER, Module.USER, "00", "can not find user"),
    USER_UPDATE_FAIL(Level.SERVER, Module.USER, "01", "user update failed"),

    /**
     * 家庭错误码.
     */
    FAMILY_NOT_FOUND(Level.SERVER, Module.FAMILY, "00", "can not find family"),
    FAMILY_UPDATE_FAIL(Level.SERVER, Module.FAMILY, "01", "family update failed"),
    FAMILY_DELETE_FAIL(Level.SERVER, Module.FAMILY, "02", "family delete failed"),
    FAMILY_MEMBER_SAVE_FAIL(Level.SERVER, Module.FAMILY, "03", "family member save failed"),
    FAMILY_MEMBER_DELETE_FAIL(Level.SERVER, Module.FAMILY, "04", "family member delete failed"),
    FAMILY_MEMBER_CANNOT_CREATOR_FAIL(Level.SERVER, Module.FAMILY, "05", "family member can not creator failed"),
    FAMILY_EXIST(Level.SERVER, Module.FAMILY, "06", "Current user already have a family"),

    /**
     * 设备错误码.
     */
    DEVICE_NOT_FOUND(Level.SERVER, Module.DEVICE, "00", "can not find device"),
    DEVICE_UPDATE_FAIL(Level.SERVER, Module.DEVICE, "01", "device update failed"),
    DEVICE_DELETE_FAIL(Level.SERVER, Module.DEVICE, "02", "device delete failed"),
    DEVICE_EXIST_FAIL(Level.SERVER, Module.DEVICE, "03", "device has existed falied"),
    DEVICE_SHARE_FAIL(Level.SERVER, Module.DEVICE, "04", "device share failed"),
    DEVICE_UNSHARE_FAIL(Level.SERVER, Module.DEVICE, "05", "device unshare failed"),
    DEVICE_SHARE_OWNDEVICE_FAIL(Level.SERVER, Module.DEVICE, "06", "fail to share own device"),
    DEVICE_SAVE_FAIL(Level.SERVER, Module.DEVICE, "07", "device save failed"),
    DEVICE_ADDIOTCLOUD_FAIL(Level.SERVER, Module.DEVICE, "08", "add device to iot cloud failed"),
    DEVICE_TYPE_NOT_EXIST_FAIL(Level.SERVER, Module.DEVICE, "09", "device type not existed failed"),
    DEVICE_DELETEIOTCLOUD_FAIL(Level.SERVER, Module.DEVICE, "10", "delete device to iot cloud failed"),
    DEVICE_SEND_COMMAND_FAIL(Level.SERVER, Module.DEVICE, "11", "device send iot command failed"),

    /**
     * 设备错误码.
     */
    SCHEDULE_NOT_FOUND(Level.SERVER, Module.SCHEDULE, "00", "can not find schedule"),
    SCHEDULE_UPDATE_FAIL(Level.SERVER, Module.SCHEDULE, "01", "device update failed"),
    SCHEDULE_DELETE_FAIL(Level.SERVER, Module.SCHEDULE, "02", "device schedule failed"),
    SCHEDULE_SEND_COMMAND_FAIL(Level.SERVER, Module.DEVICE, "03", "schedule send iot command failed"),
    SCHEDULE_USER_SAVE_FAIL(Level.SERVER, Module.SCHEDULE, "04", "user schedule save failed"),

    /**
     * 数据库操作异常.
     */
    DATABASE(Level.SYSTEM, Module.DATABASE, "00", "database error"),

    /**
     * 未知错误.
     */
    UNKNOWN(Level.SYSTEM, Module.UNKNOWN, "00", "unknown error");

    private final String level;
    private final String module;
    private final String code;
    private final String description;

    ErrorCode(String level, String module, String code, String description) {
        this.level = level;
        this.module = module;
        this.code = code;
        this.description = description;
    }

    public String getLevel() {
        return level;
    }

    public String getModule() {
        return module;
    }

    public String getCode() {
        return code;
    }

    public String getDescription() {
        return description;
    }

    @Override
    public String toString() {
        return String.format(Locale.ENGLISH, "E%s%s%s", this.level, this.module, this.code);
    }

    public String toDetailString() {
        return String.format(Locale.ENGLISH, "%-20s %s   %s", Module.getModuleName(this.getModule()), this.toString(), this.description);
    }

    public static String dumpErrorCodes() {
        return Stream.of(values())
                .map(ErrorCode::toString)
                .collect(Collectors.joining("\n"));
    }

    public static String dumpDetailErrorCodes() {
        return Stream.of(values())
                .map(ErrorCode::toDetailString)
                .collect(Collectors.joining("\n"));
    }

    /**
     * 错误级别定义.
     */
    public static class Level {
        /**
         * 系统级错误， 比如IO请求失败，服务请求拒绝，多为不可预测的错误.
         */
        static final String SYSTEM = "1";
        /**
         * 服务级错误，多为业务逻辑不满足要求，比如工程不存在，没有权限等等.
         */
        static final String SERVER = "2";
    }

    /**
     * 错误模块定义.
     */
    @SuppressWarnings("serial")
    public static class Module {
        public static final String USER = "00";
        public static final String FAMILY = "01";
        public static final String DEVICE = "02";
        public static final String SCHEDULE = "03";
        public static final String SCENCE = "04";
        public static final String DATABASE = "05";
        public static final String IOT_DATABASE = "06";
        public static final String COMMON = "10";

        public static final String UNKNOWN = "99";

        public static final Map<String, String> MODULE_NAMES = Collections.unmodifiableMap(new HashMap<String, String>() {{
            put(USER, "user");
            put(FAMILY, "family");
            put(DEVICE, "device");
            put(SCHEDULE, "schedule");
            put(SCENCE, "scence");
            put(IOT_DATABASE, "iot_database");
            put(DATABASE, "database");
            put(COMMON, "common");
            put(UNKNOWN, "unknown");
        }});

        public static String getModuleName(String moduleCode) {
            return MODULE_NAMES.getOrDefault(moduleCode, "unknown");
        }
    }
}
