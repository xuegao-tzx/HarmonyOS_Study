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
 * 智能风扇
 *
 * @since 2021-08-28
 */
public class FanAbilitySlice extends AbilitySlice {
    private boolean isFanON = false;
    private Text timing;
    private Text speed;
    private Text switchState;
    private Image switchImg;
    private Image speedOneImg;
    private Image speedTwoImg;
    private Image speedThreeImg;
    private Image speedFourImg;

    private String deviceId;
    private boolean isSetCommand = false;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_fan);
        deviceId = intent.getStringParam("deviceId");
        isSetCommand = intent.getBooleanParam(KEY_IS_SET_COMMAND, false);
        initComponents();
        initDeviceState();
    }

    private void initWateringState() {
        TaskDispatcher globalTaskDispatcher = getGlobalTaskDispatcher(TaskPriority.DEFAULT);
        globalTaskDispatcher.asyncDispatch(() -> {
            KLog.i("async task initLampState run");
            getDeviceInfo();
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
                            isFanON = DeviceStateUtil.getSwitchOnStatus(property, "FanStatus");
                            String value = DeviceStateUtil.getKeyValue(property, "SpeedLevel");
                            speedOneImg.setPixelMap(ResourceTable.Media_icon_gear_one_unselected);
                            speedTwoImg.setPixelMap(ResourceTable.Media_icon_gear_two_unselected);
                            speedThreeImg.setPixelMap(ResourceTable.Media_icon_gear_three_unselected);
                            speedFourImg.setPixelMap(ResourceTable.Media_icon_gear_four_unselected);
                            if (isFanON) {
                                switchImg.setPixelMap(ResourceTable.Media_icon_switch_on);
                                switch (value) {
                                    case "1": {
                                        speed.setText("1");
                                        speedOneImg.setPixelMap(ResourceTable.Media_icon_gear_one_selected);
                                        break;
                                    }
                                    case "2": {
                                        speed.setText("2");
                                        speedTwoImg.setPixelMap(ResourceTable.Media_icon_gear_two_selected);
                                        break;
                                    }
                                    case "3": {
                                        speed.setText("3");
                                        speedThreeImg.setPixelMap(ResourceTable.Media_icon_gear_three_selected);
                                        break;
                                    }
                                    case "4": {
                                        speed.setText("4");
                                        speedFourImg.setPixelMap(ResourceTable.Media_icon_gear_four_selected);
                                        break;
                                    }

                                }

                            } else {
                                switchImg.setPixelMap(ResourceTable.Media_icon_switch_off);
                                speed.setText("0");
                            }

                        } else {
                            initWateringState();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        // 失败是因为首次配网状态未更新
                        initWateringState();
                    }
                });
    }

    private void getDeviceInfo() {
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
        switchImg = (Image) findComponentById(ResourceTable.Id_fan_image_switch);
        switchImg.setClickedListener(component -> switchListener());
        switchState = (Text) findComponentById(ResourceTable.Id_fan_text_switch);

        findComponentById(ResourceTable.Id_fan_img_left).setClickedListener(this::goBack);

        speed = (Text) findComponentById(ResourceTable.Id_fan_text_speed);
        speedOneImg = (Image) findComponentById(ResourceTable.Id_fan_img_speed_one);
        speedOneImg.setClickedListener(component -> changeSpeed("one"));

        speedTwoImg = (Image) findComponentById(ResourceTable.Id_fan_img_speed_two);
        speedTwoImg.setClickedListener(component -> changeSpeed("two"));

        speedThreeImg = (Image) findComponentById(ResourceTable.Id_fan_img_speed_three);
        speedThreeImg.setClickedListener(component -> changeSpeed("three"));

        speedFourImg = (Image) findComponentById(ResourceTable.Id_fan_img_speed_four);
        speedFourImg.setClickedListener(component -> changeSpeed("four"));

        timing = (Text) findComponentById(ResourceTable.Id_fan_text_timing);
        Slider slider = (Slider) findComponentById(ResourceTable.Id_fan_slider_timing);
        slider.setValueChangedListener(new Slider.ValueChangedListener() {
            @Override
            public void onProgressUpdated(Slider slider, int progress, boolean fromUser) {

            }

            @Override
            public void onTouchStart(Slider slider) {

            }

            @Override
            public void onTouchEnd(Slider slider) {
                timing.setText(String.format(Locale.CHINA, "%s", slider.getProgress()));
            }
        });

        // 顶部右侧图标
        if (isSetCommand) {
            Image rightIcon = (Image) findComponentById(ResourceTable.Id_fan_img_right);
            rightIcon.setPixelMap(ResourceTable.Media_icon_ok);
            rightIcon.setClickedListener(this::saveCommand);
        }
    }


    private void saveCommand(Component component) {
        EventBus.getDefault().post(new MessageEvent(MessageEvent.EVENT_GET_COMMAND, deviceId, CommandUtil.getFanStatusCommand(isFanON, Integer.parseInt(speed.getText()))));
        onBackPressed();
    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void changeSpeed(String value) {
        speedOneImg.setPixelMap(ResourceTable.Media_icon_gear_one_unselected);
        speedTwoImg.setPixelMap(ResourceTable.Media_icon_gear_two_unselected);
        speedThreeImg.setPixelMap(ResourceTable.Media_icon_gear_three_unselected);
        speedFourImg.setPixelMap(ResourceTable.Media_icon_gear_four_unselected);
        switch (value) {
            case "one": {
                speedOneImg.setPixelMap(ResourceTable.Media_icon_gear_one_selected);
                speed.setText("1");
                // 是设置日程命令还是发送命令
                if (!isSetCommand) {
                    sendCommand(CommandUtil.getFanSpeedCommand(1));
                }
                break;
            }
            case "two": {
                speedTwoImg.setPixelMap(ResourceTable.Media_icon_gear_two_selected);
                speed.setText("2");
                // 是设置日程命令还是发送命令
                if (!isSetCommand) {
                    sendCommand(CommandUtil.getFanSpeedCommand(2));
                }
                break;
            }
            case "three": {
                speedThreeImg.setPixelMap(ResourceTable.Media_icon_gear_three_selected);
                speed.setText("3");
                // 是设置日程命令还是发送命令
                if (!isSetCommand) {
                    sendCommand(CommandUtil.getFanSpeedCommand(3));
                }
                break;
            }
            case "four": {
                speedFourImg.setPixelMap(ResourceTable.Media_icon_gear_four_selected);
                speed.setText("4");
                // 是设置日程命令还是发送命令
                if (!isSetCommand) {
                    sendCommand(CommandUtil.getFanSpeedCommand(4));
                }
                break;
            }
        }
    }

    private void switchListener() {
        isFanON = !isFanON;
        if (isFanON) {
            switchImg.setPixelMap(ResourceTable.Media_icon_switch_on);
            if ("0".equals(speed.getText())) {
                speed.setText("2");
                speedTwoImg.setPixelMap(ResourceTable.Media_icon_gear_two_selected);
            }
            switchState.setText("已开启");

        } else {
            speedOneImg.setPixelMap(ResourceTable.Media_icon_gear_one_unselected);
            speedTwoImg.setPixelMap(ResourceTable.Media_icon_gear_two_unselected);
            speedThreeImg.setPixelMap(ResourceTable.Media_icon_gear_three_unselected);
            speedFourImg.setPixelMap(ResourceTable.Media_icon_gear_four_unselected);
            switchImg.setPixelMap(ResourceTable.Media_icon_switch_off);
            switchState.setText("已关闭");
            speed.setText("0");
        }
        // 是设置日程命令还是发送命令
        if (!isSetCommand) {
            sendCommand(CommandUtil.getFanStatusCommand(isFanON, Integer.parseInt(speed.getText())));
        }
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
                            XToastUtils.toast("发送控制命令成功");
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
