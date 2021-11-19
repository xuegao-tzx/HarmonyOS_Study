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

import com.example.distschedule.ImageSelectorAbility;
import com.example.distschedule.LoginAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.utils.PreferencesUtil;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.*;
import ohos.data.preferences.Preferences;

public class PersonalInformationAbilitySlice extends AbilitySlice implements Component.ClickedListener {

    private Text name;

    @Override
    protected void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_personalinfor);
        initComponents();
    }

    private void initComponents() {
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_personalinfor_dl_back);
        DirectionalLayout dlIcon = (DirectionalLayout) findComponentById(ResourceTable.Id_personalinfor_dl_icon);
        DirectionalLayout dlAccount = (DirectionalLayout) findComponentById(ResourceTable.Id_personalinfor_dl_account);
        DirectionalLayout dlQRCode = (DirectionalLayout) findComponentById(ResourceTable.Id_personalinfor_dl_qrcode);
        findComponentById(ResourceTable.Id_personalinfor_dl_qrcode);
        name = (Text) findComponentById(ResourceTable.Id_personalInfo_text_name);
        Button loginOut = (Button) findComponentById(ResourceTable.Id_personalinfor_bt_loginout);

        dlBack.setClickedListener(this);
        dlIcon.setClickedListener(this);
        dlAccount.setClickedListener(this);
        dlQRCode.setClickedListener(this);
        loginOut.setClickedListener(this);
    }

    @Override
    protected void onActive() {
        super.onActive();
        name.setText(PreferencesUtil.getUserName(getContext()));
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_personalinfor_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_personalinfor_dl_icon:
                Intent intent = new Intent();
                Operation operation =
                        new Intent.OperationBuilder()
                                .withBundleName(getBundleName())
                                .withAbilityName(ImageSelectorAbility.class.getName())
                                .build();
                intent.setOperation(operation);
                startAbility(intent, 0);
                break;
            case ResourceTable.Id_personalinfor_dl_account:
                present(new AccountNameChangeAbilitySlice(), new Intent());
                break;
            case ResourceTable.Id_personalinfor_dl_qrcode:
                present(new MyQrCodeAbilitySlice(), new Intent());
                break;
            case ResourceTable.Id_personalinfor_bt_loginout:
                loginOut();
                break;
            default:
                break;
        }
    }

    private void loginOut() {
        Preferences preferences = PreferencesUtil.getPreferences(getContext());
        preferences.clear();
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(getBundleName())
                        .withAbilityName(LoginAbility.class.getName())
                        .build();
        intent.setOperation(operation);
        startAbility(intent, 0);
        onBackPressed();
    }
}
