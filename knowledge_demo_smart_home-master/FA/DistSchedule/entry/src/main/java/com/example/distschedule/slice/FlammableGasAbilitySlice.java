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
/*
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
*/
/**
 * 可燃气体控制页面
 *
 * @since 2021-08-28
 *
public class FlammableGasAbilitySlice extends AbilitySlice {
    private boolean isAlertFlammableGasON = false;
    private DirectionalLayout commonFlammableGasControl;
    private DirectionalLayout alertFlammableGasControl;
    private Image logoFlammableGasImg;

    private String deviceId;
    private Slider thresholdFlammableGasSlider;

    private Text currentFlammableGasValue;
    private Text thresholdFlammableGasValue;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_flammable_gas);

        deviceId = intent.getStringParam("deviceId");
        initComponents();
        initDeviceState();
    }

    private void initFlammableGasState() {
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
                            isAlertFlammableGasON = DeviceStateUtil.getSwitchOnStatus(property, "BeepStatus");
                            String value = DeviceStateUtil.getKeyValue(property, "MQ5");
                            String threshold = DeviceStateUtil.getKeyValue(property, "Threshold");
                            if (isAlertFlammableGasON) {
                                // 告警页面
                                goToAlert();
                            }
                            currentFlammableGasValue.setText(String.format("%.2f", Double.parseDouble(value)));
                            thresholdFlammableGasValue.setText(String.format("%s", threshold));
                            thresholdFlammableGasSlider.setProgressValue(Integer.parseInt(threshold));

                        } else {
                            initFlammableGasState();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        initFlammableGasState();
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
        logoFlammableGasImg = (Image) findComponentById(ResourceTable.Id_flammablegas_image_logo);
        commonFlammableGasControl = (DirectionalLayout) findComponentById(ResourceTable.Id_flammablegas_common_show);
        alertFlammableGasControl = (DirectionalLayout) findComponentById(ResourceTable.Id_flammablegas_alert_show);

        findComponentById(ResourceTable.Id_flammablegas_alert_stop).setClickedListener(component -> switchListener());

        findComponentById(ResourceTable.Id_flammablegas_img_left).setClickedListener(this::goBack);
        currentFlammableGasValue = (Text) findComponentById(ResourceTable.Id_flammablegas_current_value);
        thresholdFlammableGasValue = (Text) findComponentById(ResourceTable.Id_flammablegas_text_threshold);
        thresholdFlammableGasSlider = (Slider) findComponentById(ResourceTable.Id_flammablegas_slider_threshold);
        thresholdFlammableGasSlider.setValueChangedListener(new Slider.ValueChangedListener() {
            @Override
            public void onProgressUpdated(Slider slider, int progress, boolean fromUser) {
            }

            @Override
            public void onTouchStart(Slider slider) {

            }

            @Override
            public void onTouchEnd(Slider slider) {
                thresholdFlammableGasValue.setText(String.format(Locale.CHINA, "%s", slider.getProgress()));
                sendCommand(CommandUtil.getFlammableGasThresholdCommand(slider.getProgress()));
            }
        });
    }

    //切换告警页面
    private void goToAlert() {
        logoFlammableGasImg.setPixelMap(ResourceTable.Media_icon_device_alert);
        commonFlammableGasControl.setVisibility(Component.HIDE);
        alertFlammableGasControl.setVisibility(Component.VISIBLE);

    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void switchListener() {
        isAlertFlammableGasON = !isAlertFlammableGasON;
        if (!isAlertFlammableGasON) {
            //回到正常控制页面
            logoFlammableGasImg.setPixelMap(ResourceTable.Media_icon_flammablegas);
            commonFlammableGasControl.setVisibility(Component.VISIBLE);
            alertFlammableGasControl.setVisibility(Component.HIDE);
        }
        // 手动开关烟雾报警
        sendCommand(CommandUtil.getFlammableGasStatusCommand(isAlertFlammableGasON));
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
*/