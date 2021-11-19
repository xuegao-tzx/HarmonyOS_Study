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
import com.example.distschedule.event.MessageEvent;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.CommandUtil;
import com.example.distschedule.utils.DeviceStateUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Component;
import ohos.agp.components.Image;
import ohos.agp.components.Slider;
import ohos.agp.components.Text;
import ohos.app.dispatcher.TaskDispatcher;
import ohos.app.dispatcher.task.TaskPriority;
import ohos.miscservices.timeutility.Time;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.greenrobot.eventbus.EventBus;

import java.util.Locale;

import static com.example.distschedule.slice.SelectDeviceAbilitySlice.KEY_IS_SET_COMMAND;

/**
 * 电灯控制页面
 *
 * @since 2021-08-28
 */
public class LampAbilitySlice extends AbilitySlice {
    private boolean isON = false;
    private String deviceId;
    private Slider dutyCycleSlider;
    private Text lampState;
    private Image switchImg;
    private Text lampBright;
    private boolean isSetCommand = false;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_lamp);

        deviceId = intent.getStringParam("deviceId");
        isSetCommand = intent.getBooleanParam(KEY_IS_SET_COMMAND, false);
        initComponents();
        initDeviceState();
    }

    private void initLampState() {
        TaskDispatcher globalTaskDispatcher = getGlobalTaskDispatcher(TaskPriority.DEFAULT);
        globalTaskDispatcher.asyncDispatch(() -> {
            KLog.i("async task initLampState run");
            checkDeviceNet();
        });
    }

    private void initDeviceState() {
        // 从服务端获取设备信息
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        KLog.i(deviceInfo);
                        if (deviceInfo.isSuccess() && deviceInfo.getResult().isOnline()) {
                            // 更新状态
                            String property = deviceInfo.getResult().getProperty();
                            isON = DeviceStateUtil.getLampStatus(property);
                            String brightness = DeviceStateUtil.getDutyCycle(property);
                            if (isON) {
                                switchImg.setPixelMap(ResourceTable.Media_icon_switch_on);
                                lampState.setText("已开启");
                                lampBright.setText(String.format(Locale.CHINA, "%s", brightness + "%"));
                                dutyCycleSlider.setProgressValue(Integer.parseInt(brightness));

                            } else {
                                switchImg.setPixelMap(ResourceTable.Media_icon_switch_off);
                                lampState.setText("已关闭");
                                lampBright.setText(String.format(Locale.CHINA, "%s", 0 + "%"));
                                dutyCycleSlider.setProgressValue(0);
                            }

                        } else {
                            initLampState();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        initLampState();
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
        switchImg = (Image) findComponentById(ResourceTable.Id_lamp_image_switch);
        switchImg.setClickedListener(component -> switchListener());
        findComponentById(ResourceTable.Id_lamp_img_left).setClickedListener(this::goBack);
        lampState = (Text) findComponentById(ResourceTable.Id_lamp_text_switch);
        lampBright = (Text) findComponentById(ResourceTable.Id_lamp_text_bright);
        dutyCycleSlider = (Slider) findComponentById(ResourceTable.Id_lamp_slider_bright);
        dutyCycleSlider.setValueChangedListener(new Slider.ValueChangedListener() {
            @Override
            public void onProgressUpdated(Slider slider, int progress, boolean fromUser) {

            }

            @Override
            public void onTouchStart(Slider slider) {

            }

            @Override
            public void onTouchEnd(Slider slider) {
                lampBright.setText(String.format(Locale.CHINA, "%s", slider.getProgress() + "%"));
                if (!isSetCommand) {
                    sendCommand(CommandUtil.getLampDutyCycleCommand(slider.getProgress()));
                }
            }
        });

        // 顶部右侧图标
        if (isSetCommand) {
            Image rightIcon = (Image) findComponentById(ResourceTable.Id_lamp_img_right);
            rightIcon.setPixelMap(ResourceTable.Media_icon_ok);
            rightIcon.setClickedListener(this::saveCommand);
        }

    }

    private void saveCommand(Component component) {
        // 将device和命令使用EventBus传出到 SelectDeviceAbilitySlice中，用于保存命令
        EventBus.getDefault().post(new MessageEvent(MessageEvent.EVENT_GET_COMMAND,
                deviceId, CommandUtil.getLampStatusCommand(isON, dutyCycleSlider.getProgress())));
        onBackPressed();
    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void switchListener() {
        isON = !isON;
        if (isON) {
            switchImg.setPixelMap(ResourceTable.Media_icon_switch_on);
            lampState.setText("已开启");
        } else {
            switchImg.setPixelMap(ResourceTable.Media_icon_switch_off);
            lampState.setText("已关闭");
        }

        // 是设置日程命令还是发送命令
        if (!isSetCommand) {
            sendCommand(CommandUtil.getLampStatusCommand(isON, dutyCycleSlider.getProgress()));
        }
    }

    /**
     * 发送命令
     *
     * @param command 命令
     */
    private void sendCommand(Command command) {
        JSONObject body = new JSONObject();
        body.put("commandName", command.getCommandName());
        body.put("serviceId", command.getServiceId());
        body.put("value", command.getValue());
        OKHttpUtilsRx2.INSTANCE.getApi().sendCommand(deviceId,
                RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
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
