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

import com.example.distschedule.DeviceControlAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.utils.CommandUtil;

import java.util.HashMap;
import java.util.Map;

public enum DeviceData {
    INSTANCE;
    private static Map<String, DeviceDataBean> data;

    public Map<String, DeviceDataBean> getDeviceData() {
        if (data == null) {
            initData();
        }
        return data;
    }

    private void initData() {
        data = new HashMap<>();
        // 灯
        data.put("6128c7b60ad1ed0286680f19", new DeviceDataBean("", ResourceTable.Media_icon_fraction_lamp_online,
                ResourceTable.Media_icon_fraction_lamp_offline,
                command -> CommandUtil.parseLampCommand(command)));
        /* 扫地机器人
        data.put("61371e952cce4f0286262229", new DeviceDataBean(DeviceControlAbility.ACTION_SWEEPING_ROBOT,
                ResourceTable.Media_icon_fraction_robot_online, ResourceTable.Media_icon_fraction_robot_offline,
                command -> CommandUtil.parseRobotCleanCommand(command)));
        // 护花使者
        data.put("6135e8fc2cce4f028625ccc9", new DeviceDataBean(DeviceControlAbility.ACTION_WATERING,
                ResourceTable.Media_icon_fraction_watering_online, ResourceTable.Media_icon_fraction_watering_offline,
                command -> CommandUtil.parseWateringCommand(command)));
        // 窗帘
        data.put("6136ceba0ad1ed02866fa3b2", new DeviceDataBean(DeviceControlAbility.ACTION_CURTAIN,
                ResourceTable.Media_icon_fraction_curtain_online, ResourceTable.Media_icon_fraction_curtain_offline,
                command -> CommandUtil.parseCurtainCommand(command)));
        *///烟感
        data.put("6128bdd12cce4f02861e6d98", new DeviceDataBean(DeviceControlAbility.ACTION_SMOKE_DETECTOR,
                ResourceTable.Media_icon_fraction_smoke_online, ResourceTable.Media_icon_fraction_smoke_offline, null));
        /* 可燃气体
        data.put("61445c41f74139027d2233e5", new DeviceDataBean(DeviceControlAbility.ACTION_FLAMMABLE_GAS,
                ResourceTable.Media_icon_fraction_gas_online, ResourceTable.Media_icon_fraction_gas_offline, null));
        // 甲醛检测
        data.put("61445cdadcbb87027db095d1", new DeviceDataBean(DeviceControlAbility.ACTION_FORMALDEHYDE,
                ResourceTable.Media_icon_fraction_formaldehyde_on, ResourceTable.Media_icon_fraction_formaldehyde_offline, null));
        // 智能垃圾桶
        data.put("614456cff74139027d2230ec", new DeviceDataBean(DeviceControlAbility.ACTION_TRASHCAN,
                ResourceTable.Media_icon_fraction_trash_online, ResourceTable.Media_icon_fraction_trash_offline, null));
        // 儿童手表
        data.put("613ad28b0109930287315230", new DeviceDataBean(DeviceControlAbility.ACTION_CHILDREN_WATCH,
                ResourceTable.Media_icon_fraction_watch_online, ResourceTable.Media_icon_fraction_watch_offline, null));
        */// 电风扇
        data.put("6150601d88056b027dd2ca47", new DeviceDataBean(DeviceControlAbility.ACTION_FAN,
                ResourceTable.Media_icon_fraction_fan_online, ResourceTable.Media_icon_fraction_fan_offline,
                command -> CommandUtil.parseFanCommand(command)));
    }
}
