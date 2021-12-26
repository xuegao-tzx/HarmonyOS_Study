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

package com.example.distschedule;

import com.example.distschedule.slice.*;
import ohos.aafwk.ability.Ability;
import ohos.aafwk.content.Intent;

/**
 * 控制页面
 *
 * @since 2021-08-28
 */
public class DeviceControlAbility extends Ability {
    public static final String ACTION_WATERING = "action.watering";
    public static final String ACTION_SWEEPING_ROBOT = "action.sweepingRobot";
    public static final String ACTION_CURTAIN = "action.curtain";
    public static final String ACTION_FAN = "action.fan";
    public static final String ACTION_CHILDREN_WATCH = "action.childrenWatch";
    public static final String ACTION_TRASHCAN = "action.trashCan";
    public static final String ACTION_SMOKE_DETECTOR = "action.smokeDetector";
    public static final String ACTION_FORMALDEHYDE = "action.formaldehyde";
    public static final String ACTION_FLAMMABLE_GAS = "action.flammableGas";

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setMainRoute(LampAbilitySlice.class.getName()); // 智能台灯
        //addActionRoute(ACTION_WATERING, WateringAbilitySlice.class.getName()); // 护花使者
        //addActionRoute(ACTION_SWEEPING_ROBOT, SweepingRobotAbilitySlice.class.getName()); // 智能扫地机
        //addActionRoute(ACTION_CURTAIN, CurtainAbilitySlice.class.getName()); // 窗帘控制
        //addActionRoute(ACTION_FAN, FanAbilitySlice.class.getName()); // 智能风扇
        //addActionRoute(ACTION_CHILDREN_WATCH, ChildrenWatchAbilitySlice.class.getName()); // 智能儿童手表

        //addActionRoute(ACTION_TRASHCAN, TrashCanAbilitySlice.class.getName()); // 智能垃圾桶
        addActionRoute(ACTION_SMOKE_DETECTOR, SmokeDetectorAbilitySlice.class.getName()); // 烟感控制
        //addActionRoute(ACTION_FORMALDEHYDE, FormaldehydeAbilitySlice.class.getName()); // 甲醛检测
        //addActionRoute(ACTION_FLAMMABLE_GAS, FlammableGasAbilitySlice.class.getName()); // 可燃气体
    }
}
