/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import com.example.distschedule.MainAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.DevicesType;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.UserResult;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.DeviceInfoUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.Component;
import ohos.agp.components.TextField;
import ohos.distributedhardware.devicemanager.DeviceInfo;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.apache.commons.lang3.StringUtils;

import java.net.SocketException;
import java.util.List;

/**
 * 登录页面
 *
 * @since 2021-08-28
 */
public class LoginAbilitySlice extends AbilitySlice {
    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_login);
        if (!StringUtils.isBlank(PreferencesUtil.getUserId(getContext()))) {
            Intent mainIntent = new Intent();
            Operation operation =
                    new Intent.OperationBuilder()
                            .withBundleName(getBundleName())
                            .withAbilityName(MainAbility.class.getName())
                            .build();
            mainIntent.setOperation(operation);
            startAbility(mainIntent);
            terminateAbility();
        }
        initComponents();
        initDeviceType();
    }

    @Override
    protected void onBackPressed() {
        if (StringUtils.isBlank(PreferencesUtil.getUserId(getContext()))) {
            XToastUtils.warning("请先登录");
        } else {
            terminate();
        }
    }

    private void initComponents() {
        findComponentById(ResourceTable.Id_login_button_login).setClickedListener(this::login);
    }

    private void login(Component component) {
        TextField tfPhone = (TextField) findComponentById(ResourceTable.Id_login_text_number);
        String phone = tfPhone.getText();
        if (StringUtils.isBlank(phone)) {
            XToastUtils.warning("请输入手机号");
            return;
        } else if (phone.length() != 11) {
            XToastUtils.warning("请输入11位手机号");
            return;
        }
        OKHttpUtilsRx2.INSTANCE.getApi().getUserInfo(tfPhone.getText())
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<UserResult>>() {
                    @Override
                    public void onNext(RespondBase<UserResult> userInfo) {
                        super.onNext(userInfo);
                        loginSuccess(userInfo);
                    }
                });
    }

    private void initDeviceType() {
        OKHttpUtilsRx2.INSTANCE.getApi().getDevicesType()
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<DevicesType>>>() {
                    @Override
                    public void onNext(RespondBase<List<DevicesType>> respond) {
                        super.onNext(respond);
                        List<DevicesType> devicesTypeList = respond.getResult();
                        PreferencesUtil.putDeviceType(getContext(), devicesTypeList);
                    }
                });
    }

    private void loginSuccess(RespondBase<UserResult> userInfo) {
        KLog.i(userInfo);
        PreferencesUtil.putUserInfo(getContext(), userInfo.getResult().getId(), userInfo.getResult().getName(), userInfo.getResult().getPhone());
        PreferencesUtil.getUserId(getContext());
        XToastUtils.toast("登录成功：" + userInfo.getResult().getPhone());
        saveHandDevice(userInfo.getResult());
    }


    /**
     * 保存手持设备
     *
     * @param userInfo 用户信息
     */
    private void saveHandDevice(UserResult userInfo) {
        String deviceId = null;
        try {
            deviceId = DeviceInfoUtil.getMacAddress(this);
        } catch (SocketException e) {
            e.printStackTrace();
        }
        DeviceInfo.DeviceType deviceType = DeviceInfoUtil.getDeviceType(this);
        KLog.i("deviceId = " + deviceId);
        KLog.i("deviceType = " + deviceType.name());

        if (DeviceInfo.DeviceType.PHONE.equals(deviceType) ||
                DeviceInfo.DeviceType.TABLET.equals(deviceType) ||
                DeviceInfo.DeviceType.WEARABLE.equals(deviceType)) {
            //调用创建设备接口,创建手持设备
            JSONObject body = new JSONObject();
            body.put("id", deviceId);
            body.put("name", deviceType.name().toLowerCase());
            body.put("ownerId", userInfo.getId());
            switch (deviceType) {
                case PHONE:
                    body.put("typeId", 5);//手机类型
                    break;
                case TABLET:
                    body.put("typeId", 7);//平板类型
                    break;
                default:
                    body.put("typeId", 6);//手持设备类型
                    break;
            }
            OKHttpUtilsRx2.INSTANCE.getApi().addDevice(RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
                    .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                    .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                        @Override
                        public void onNext(RespondBase deviceInfo) {
                            super.onNext(deviceInfo);
                            KLog.i(deviceInfo);
                            if (deviceInfo.isSuccess()) {
                                Intent loginIntent = new Intent();
                                Operation operation = new Intent.OperationBuilder()
                                        .withBundleName(getBundleName())
                                        .withAbilityName(MainAbility.class.getName())
                                        .build();
                                loginIntent.setOperation(operation);
                                startAbility(loginIntent, 0);
                            }
                        }
                    });
        } else {
            KLog.e("Invalid device type:" + deviceType.name());
            XToastUtils.toast("无效的设备类型" + deviceType.name());
        }
    }

    @Override
    public void onActive() {
        super.onActive();
        if (!StringUtils.isBlank(PreferencesUtil.getUserId(getContext()))) {
            terminate();
        }
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }
}
