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

package com.example.distschedule.slice;

import com.example.distschedule.ResourceTable;
import com.example.distschedule.utils.PreferencesUtil;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.DependentLayout;
import ohos.agp.components.Image;
import ohos.agp.components.Text;

public class CreateFamilySuccessAbilitySlice extends AbilitySlice {
    private String familyId;
    private String creatorId;
    private String familyName;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_family_create_success);
        if (intent != null) {
            familyId = intent.getStringParam("familyId");
            creatorId = intent.getStringParam("creatorId");
            familyName = intent.getStringParam("familyName");
        }
        initComponents();
    }

    private void initComponents() {
        Text familyNameText = (Text) this.findComponentById(ResourceTable.Id_text_familyName);
        familyNameText.setText(PreferencesUtil.getUserFamilyName(this));
        Image backImage = (Image) this.findComponentById(ResourceTable.Id_icon_back);
        DependentLayout addMember = (DependentLayout) this.findComponentById(ResourceTable.Id_familyCreateSuccess_dl_addMember);
        DependentLayout setFamily = (DependentLayout) this.findComponentById(ResourceTable.Id_familyCreateSuccess_dl_setFamily);
        backImage.setClickedListener(component ->
        {
            onBackPressed();
        });

        addMember.setClickedListener(component -> {
            Intent intent = new Intent();
            intent.setParam("familyId", familyId);
            intent.setParam("creatorId", creatorId);
            present(new AddMemberAbilitySlice(), intent);
        });


        setFamily.setClickedListener(component -> {
            Intent intent = new Intent();
            intent.setParam("familyId", familyId);
            intent.setParam("creatorId", creatorId);
            intent.setParam("familyName", familyName);
            present(new MyFamilyAbilitySlice(), intent);
        });
    }
}
