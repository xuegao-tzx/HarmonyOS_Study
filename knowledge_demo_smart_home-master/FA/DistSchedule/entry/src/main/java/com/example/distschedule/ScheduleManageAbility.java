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
 * 日程管理页面
 *
 * @since 2021-08-28
 */
public class ScheduleManageAbility extends Ability {
    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setMainRoute(AddScheduleAbilitySlice.class.getName());
        addActionRoute("action.selectDevice", SelectDeviceAbilitySlice.class.getName());
        addActionRoute("action.shareDevice", ShareDeviceAbilitySlice.class.getName());
        addActionRoute("action.searchSchedule", SearchScheduleAbilitySlice.class.getName());
        addActionRoute("action.shareList", ShareListAbilitySlice.class.getName());
    }
}
