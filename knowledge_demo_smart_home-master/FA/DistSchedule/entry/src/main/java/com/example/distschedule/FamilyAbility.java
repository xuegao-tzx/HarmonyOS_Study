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

public class FamilyAbility extends Ability {
    /**
     * request Code.
     */
    private static final int REQUEST_CODE = 20210601;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setMainRoute(CreateFamilyAbilitySlice.class.getName());
        addActionRoute("action.createFamilySuccess", CreateFamilySuccessAbilitySlice.class.getName());
        addActionRoute("action.myfamily", MyFamilyAbilitySlice.class.getName());
        addActionRoute("action.personalinfor", PersonalInformationAbilitySlice.class.getName());
        addActionRoute("action.inputName", AccountNameChangeAbilitySlice.class.getName());
        addActionRoute("action.myqrcode", MyQrCodeAbilitySlice.class.getName());
        addActionRoute("action.addmember", AddMemberAbilitySlice.class.getName());
        addActionRoute("action.familyqrcode", FamilyQrCodeAbilitySlice.class.getName());
        addActionRoute("action.nickname", UserNameAbilitySlice.class.getName());
        addActionRoute("action.familyName", UserNameAbilitySlice.class.getName());
    }
}
