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
import com.example.distschedule.bean.Command;
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.CommandUtil;
import com.example.distschedule.utils.DeviceStateUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;
import ohos.app.dispatcher.TaskDispatcher;
import ohos.app.dispatcher.task.TaskPriority;
import ohos.miscservices.timeutility.Time;
import okhttp3.MediaType;
import okhttp3.RequestBody;

import java.util.Locale;


/**
 * 烟感控制页面
 *
 * @since 2021-08-28
 */
public class SmokeDetectorAbilitySlice extends AbilitySlice {
    private boolean isSmokeAlertON = false;
    private DirectionalLayout commonSmokeControl;
    private DirectionalLayout alertSmokeControl;
    private Image logoSmokeImg;

    private String deviceId;
    private Slider thresholdSmokeSlider;

    private Text currentSmokeValue;
    private Text thresholdSmokeValue;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_smoke_detector);

        deviceId = intent.getStringParam("deviceId");
        initComponents();
        initDeviceState();
    }

    private void initSmokeDetectorState() {
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
                            isSmokeAlertON = DeviceStateUtil.getSwitchOnStatus(property, "BeepStatus");
                            String value = DeviceStateUtil.getKeyValue(property, "MQ2");
                            String threshold = DeviceStateUtil.getKeyValue(property, "Threshold");

                            if (isSmokeAlertON) {
                                // 告警页面
                                goToAlert();
                            }
                            currentSmokeValue.setText(String.format("%.2f", Double.parseDouble(value)));
                            thresholdSmokeValue.setText(String.format("%s", threshold));
                            thresholdSmokeSlider.setProgressValue(Integer.parseInt(threshold));

                        } else {
                            initSmokeDetectorState();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        initSmokeDetectorState();
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
        logoSmokeImg = (Image) findComponentById(ResourceTable.Id_smoke_image_logo);
        commonSmokeControl = (DirectionalLayout) findComponentById(ResourceTable.Id_smoke_common_show);
        alertSmokeControl = (DirectionalLayout) findComponentById(ResourceTable.Id_smoke_alert_show);

        findComponentById(ResourceTable.Id_smoke_alert_stop).setClickedListener(component -> switchListener());

        findComponentById(ResourceTable.Id_smoke_img_left).setClickedListener(this::goBack);
        currentSmokeValue = (Text) findComponentById(ResourceTable.Id_smoke_current_value);
        thresholdSmokeValue = (Text) findComponentById(ResourceTable.Id_smoke_text_threshold);
        thresholdSmokeSlider = (Slider) findComponentById(ResourceTable.Id_smoke_slider_threshold);
        thresholdSmokeSlider.setValueChangedListener(new Slider.ValueChangedListener() {
            @Override
            public void onProgressUpdated(Slider slider, int progress, boolean fromUser) {
            }

            @Override
            public void onTouchStart(Slider slider) {

            }

            @Override
            public void onTouchEnd(Slider slider) {
                thresholdSmokeValue.setText(String.format(Locale.CHINA, "%s", slider.getProgress()));
                sendCommand(CommandUtil.getSmokeThresholdCommand(slider.getProgress()));
            }
        });
    }

    //切换告警页面
    private void goToAlert() {
        logoSmokeImg.setPixelMap(ResourceTable.Media_icon_device_alert);
        commonSmokeControl.setVisibility(Component.HIDE);
        alertSmokeControl.setVisibility(Component.VISIBLE);

    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void switchListener() {
        isSmokeAlertON = !isSmokeAlertON;
        if (!isSmokeAlertON) {
            //回到正常控制页面
            logoSmokeImg.setPixelMap(ResourceTable.Media_icon_smoke_detector);
            commonSmokeControl.setVisibility(Component.VISIBLE);
            alertSmokeControl.setVisibility(Component.HIDE);
        }
        // 手动开关烟雾报警
        sendCommand(CommandUtil.getSmokeStatusCommand(isSmokeAlertON));
    }

    private void sendCommand(Command command) {
        JSONObject body = new JSONObject();
        body.put("commandName", command.getCommandName());
        body.put("serviceId", command.getServiceId());
        body.put("value", command.getValue());
        OKHttpUtilsRx2.INSTANCE.getApi().sendCommand(deviceId, RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respond) {
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            XToastUtils.toast("控制命令发送成功");
                        }

                    }
                });
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

