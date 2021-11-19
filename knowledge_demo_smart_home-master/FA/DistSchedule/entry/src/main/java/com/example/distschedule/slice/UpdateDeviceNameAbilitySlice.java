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

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Text;
import ohos.agp.components.TextField;
import okhttp3.MediaType;
import okhttp3.RequestBody;

public class UpdateDeviceNameAbilitySlice extends AbilitySlice {
    private String deviceName;
    private String deviceId;
    private Text textDevice;
    private String newDeviceName;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_update_device_name);
        deviceId = intent.getStringParam("deviceId");
        deviceName = intent.getStringParam("deviceName");
        initComponents();
    }

    private void initComponents() {
        findComponentById(ResourceTable.Id_update_device_name_cancel).setClickedListener(component -> goBack());
        findComponentById(ResourceTable.Id_update_device_name_ok).setClickedListener(component -> saveName());

        findComponentById(ResourceTable.Id_update_device_name_delete).setClickedListener(component -> cleanText());

        textDevice = (TextField) findComponentById(ResourceTable.Id_update_device_name_text);
        textDevice.setText(deviceName);
    }

    private void goBack() {
        onBackPressed();
    }

    private void saveName() {
        // 保存设备新名称，并返回
        newDeviceName = textDevice.getText();
        update(deviceId, PreferencesUtil.getUserId(getContext()));
    }

    private void cleanText() {
        // 清空输入框
        textDevice.setText(" ");
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    private void update(String deviceId, String userId) {
        JSONObject body = new JSONObject();
        body.put("name", newDeviceName);
        OKHttpUtilsRx2.INSTANCE.getApi().updateUserName(deviceId, RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)), userId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respond) {
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            XToastUtils.toast("更新成功");
                        } else if ("E21003".equals(respond.getErrCode())) {
                            XToastUtils.warning("无权限更新分享给你的设备");
                        }
                        onBackPressed();
                    }
                });
    }

}


