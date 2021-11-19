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
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.DeviceStateUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Component;
import ohos.agp.components.DirectionalLayout;
import ohos.agp.components.Image;
import ohos.agp.components.Text;
import ohos.app.dispatcher.TaskDispatcher;
import ohos.app.dispatcher.task.TaskPriority;
import ohos.miscservices.timeutility.Time;

/**
 * 智能垃圾桶控制页面
 *
 * @since 2021-08-28
 */
public class TrashCanAbilitySlice extends AbilitySlice {
    private boolean isAlertTrashCanON = false;
    private DirectionalLayout commonTrashCanControl;
    private DirectionalLayout alertTrashCanControl;
    private Image logoTrashCanImg;
    private String deviceId;
    private Text currentTrashCanValue;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_trash_can);

        deviceId = intent.getStringParam("deviceId");
        initComponents();
        initDeviceState();
    }

    private void initTrashCanState() {
        TaskDispatcher globalTaskDispatcher = getGlobalTaskDispatcher(TaskPriority.DEFAULT);
        globalTaskDispatcher.asyncDispatch(() -> {
            KLog.i("async task initLampState run");
            checkDeviceNet();
        });
    }

    private void initDeviceState() {
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        KLog.i(deviceInfo);
                        if (deviceInfo.isSuccess() && deviceInfo.getResult().isOnline()) {
                            // 更新状态
                            String property = deviceInfo.getResult().getProperty();
                            String value = DeviceStateUtil.getKeyValue(property, "CapacityUsed");

                            if (value.equals("100")) {
                                // 告警页面
                                isAlertTrashCanON = true;
                                goToAlert();
                            }
                            currentTrashCanValue.setText(String.format("%s", value));

                        } else {
                            initTrashCanState();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        initTrashCanState();
                    }
                });
    }

    private void checkDeviceNet() {
        Time.sleep(10 * 1000);
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        super.onNext(deviceInfo);
                        KLog.i(deviceInfo);
                        if (deviceInfo.getResult() == null || !deviceInfo.getResult().isOnline()) {
                            XToastUtils.toast("未配网成功，请再次碰一碰重试");
                        }
                    }
                });
    }

    private void initComponents() {
        logoTrashCanImg = (Image) findComponentById(ResourceTable.Id_trash_can_image_logo);
        commonTrashCanControl = (DirectionalLayout) findComponentById(ResourceTable.Id_trash_can_common_show);
        alertTrashCanControl = (DirectionalLayout) findComponentById(ResourceTable.Id_trash_can_alert_show);

        findComponentById(ResourceTable.Id_trash_can_image_alert_stop).setClickedListener(component -> switchListener());

        findComponentById(ResourceTable.Id_trash_can_img_left).setClickedListener(this::goBack);
        currentTrashCanValue = (Text) findComponentById(ResourceTable.Id_trash_can_current_value);

    }

    //切换告警页面
    private void goToAlert() {
        logoTrashCanImg.setPixelMap(ResourceTable.Media_icon_device_alert);
        commonTrashCanControl.setVisibility(Component.HIDE);
        alertTrashCanControl.setVisibility(Component.VISIBLE);

    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void switchListener() {
        isAlertTrashCanON = !isAlertTrashCanON;
        if (!isAlertTrashCanON) {
            //回到正常控制页面
            logoTrashCanImg.setPixelMap(ResourceTable.Media_icon_trash_can);
            commonTrashCanControl.setVisibility(Component.VISIBLE);
            alertTrashCanControl.setVisibility(Component.HIDE);
        }
    }


    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }
}
