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
import ohos.agp.components.Text;
import ohos.app.dispatcher.TaskDispatcher;
import ohos.app.dispatcher.task.TaskPriority;
import ohos.miscservices.timeutility.Time;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.greenrobot.eventbus.EventBus;

import static com.example.distschedule.slice.SelectDeviceAbilitySlice.KEY_IS_SET_COMMAND;

**
 * 扫地机器人
 *
 * @since 2021-08-28
 *//*
public class SweepingRobotAbilitySlice extends AbilitySlice {
    private boolean isON = false;
    private String deviceId;
    private Text robotState;
    private Image switchImg;
    private boolean isSetCommand = false;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_sweeping_robot);
        deviceId = intent.getStringParam("deviceId");
        isSetCommand = intent.getBooleanParam(KEY_IS_SET_COMMAND, false);
        initComponents();
        initDeviceState();
    }

    private void initRobotState() {
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
                                   if (deviceInfo.isSuccess() && deviceInfo.getResult().isOnline()) {
                                       // 更新状态
                                       String property = deviceInfo.getResult().getProperty();
                                       isON = DeviceStateUtil.getSwitchOnStatus(property, "CleanerStatus");
                                       if (isON) {
                                           switchImg.setPixelMap(ResourceTable.Media_icon_switch_on);
                                           robotState.setText("已开启");

                                       } else {
                                           switchImg.setPixelMap(ResourceTable.Media_icon_switch_off);
                                           robotState.setText("已关闭");
                                       }
                                   } else {
                                       initRobotState();
                                   }
                               }

                               @Override
                               public void onError(Throwable throwable) {
                                   // 失败是因为首次配网状态未更新
                                   initRobotState();
                               }
                           }
                );
    }

    private void getDeviceInfo() {
        Time.sleep(10 * 1000);
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        super.onNext(deviceInfo);
                        if (deviceInfo.getResult() == null || !deviceInfo.getResult().isOnline()) {
                            XToastUtils.toast("未配网成功，请再次碰一碰重试");
                        }
                    }
                });
    }

    private void initComponents() {
        switchImg = (Image) findComponentById(ResourceTable.Id_robot_image_switch);
        switchImg.setClickedListener(component -> switchListener());

        findComponentById(ResourceTable.Id_robot_img_left).setClickedListener(this::goBack);
        robotState = (Text) findComponentById(ResourceTable.Id_robot_text_switch);

        // 顶部右侧图标
        if (isSetCommand) {
            Image rightIcon = (Image) findComponentById(ResourceTable.Id_robot_img_right);
            rightIcon.setPixelMap(ResourceTable.Media_icon_ok);
            rightIcon.setClickedListener(this::saveCommand);
        }
    }

    private void saveCommand(Component component) {
        EventBus.getDefault().post(new MessageEvent(MessageEvent.EVENT_GET_COMMAND, deviceId, CommandUtil.getRobotCleanStatusCommand(isON)));
        onBackPressed();
    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void switchListener() {
        isON = !isON;
        if (isON) {
            switchImg.setPixelMap(ResourceTable.Media_icon_switch_on);
            robotState.setText("已开启");
        } else {
            switchImg.setPixelMap(ResourceTable.Media_icon_switch_off);
            robotState.setText("已关闭");

        }
        if (!isSetCommand) {
            sendCommand(CommandUtil.getRobotCleanStatusCommand(isON));
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
*/