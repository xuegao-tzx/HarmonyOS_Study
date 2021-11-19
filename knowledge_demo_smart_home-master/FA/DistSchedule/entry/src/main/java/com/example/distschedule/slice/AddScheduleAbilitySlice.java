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

import cc.cloudist.acplibrary.ACProgressFlower;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.Command;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.event.MessageEvent;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.nex3z.togglebuttongroup.MultiSelectToggleGroup;
import com.nex3z.togglebuttongroup.button.LabelToggle;
import com.nex3z.togglebuttongroup.button.MarkerButton;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.*;
import ohos.agp.components.element.Element;
import ohos.agp.components.element.ElementScatter;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.components.element.StateElement;
import ohos.agp.utils.Color;
import ohos.agp.window.dialog.CommonDialog;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.apache.commons.lang3.StringUtils;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.text.SimpleDateFormat;
import java.util.*;

/**
 * 添加日程
 *
 * @since 2021-08-28
 */
public class AddScheduleAbilitySlice extends AbilitySlice {
    private static final int REQUEST_CODE = 1;
    public Map<String, Command> commandMap = new HashMap<>();
    private Text deviceNameList;
    private Text memberName;
    private MultiSelectToggleGroup multiSelectToggleGroup;
    private String scheduleDate;
    private String memberIds = "";
    private Switch timeSwitch;
    private ACProgressFlower loadingDialog;


    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_add_schedule);
        initComponents();
        EventBus.getDefault().register(this);
        Date today = new Date();
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
        scheduleDate = format.format(today);
    }

    @Override
    protected void onStop() {
        EventBus.getDefault().unregister(this);
        super.onStop();
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onShowMessageEvent(MessageEvent messageEvent) {
        KLog.i(messageEvent);
        switch (messageEvent.getKey()) {
            case MessageEvent.EVENT_GET_DEVICE_JSON: {
                JSONObject jsonObject = messageEvent.getJsonObject();
                deviceNameList.setText(jsonObject.getString("deviceNames"));
                commandMap = jsonObject.getObject("commandMap", commandMap.getClass());
                break;
            }
            case MessageEvent.EVENT_GET_MEMBER: {
                try {
                    JSONObject jsonObject = messageEvent.getJsonObject();
                    memberName.setText(jsonObject.getString("familyName"));
                    memberIds = jsonObject.getString("memberIds");
                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            }
        }
    }

    private void initComponents() {
        // 顶部按键
        findComponentById(ResourceTable.Id_schedule_img_cancel).setClickedListener(this::cancel);
        findComponentById(ResourceTable.Id_schedule_img_confirm).setClickedListener(this::confirm);

        // 选择设备按钮
        findComponentById(ResourceTable.Id_schedule_img_add_device).setClickedListener(component -> {
            presentForResult(new SelectDeviceAbilitySlice(), new Intent(), REQUEST_CODE);
        });
        //设备名称列表
        deviceNameList = (Text) findComponentById(ResourceTable.Id_schedule_text_device);

        // 提醒方式按钮
        Element selectElement = ElementScatter.getInstance(getContext()).parse(ResourceTable.Graphic_button_select);
        Element unselectElement = ElementScatter.getInstance(getContext()).parse(ResourceTable.Graphic_button_unselect);
        Button singleBtn = (Button) findComponentById(ResourceTable.Id_schedule_btn_single);
        Button repeatBtn = (Button) findComponentById(ResourceTable.Id_schedule_btn_repeat);
        singleBtn.setClickedListener(component -> {
            singleBtn.setBackground(selectElement);
            repeatBtn.setBackground(unselectElement);
            clearCheck(multiSelectToggleGroup);
            setMaxSelectCount(multiSelectToggleGroup, 0);
        });
        repeatBtn.setClickedListener(component -> {
            repeatBtn.setBackground(selectElement);
            singleBtn.setBackground(unselectElement);
            clearCheck(multiSelectToggleGroup);
            setMaxSelectCount(multiSelectToggleGroup, 7);
        });


        // 开始结束时间
        Text startTimeText = (Text) findComponentById(ResourceTable.Id_schedule_text_start);
        startTimeText.setClickedListener(component -> {
            createTimePicker((Text) component);
        });
        Text endTimeText = (Text) findComponentById(ResourceTable.Id_schedule_text_end);
        endTimeText.setClickedListener(component -> {
            createTimePicker((Text) component);
        });
        timeSwitch = (Switch) findComponentById(ResourceTable.Id_schedule_switch_allday);
        initSwitch();
        timeSwitch.setCheckedStateChangedListener((button, isChecked) -> {
            List<String> nextMinSchedule = getScheduleDate(isChecked);
            startTimeText.setText(nextMinSchedule.get(0));
            endTimeText.setText(nextMinSchedule.get(1));
        });

        // 多选
        multiSelectToggleGroup = (MultiSelectToggleGroup) findComponentById(ResourceTable.Id_schedule_mstg_checkbox);
        for (int i = 0; i < multiSelectToggleGroup.getChildCount(); i++) {
            LabelToggle labelToggle = (LabelToggle) multiSelectToggleGroup.getComponentAt(i);
            labelToggle.setMarkerColor(Color.getIntColor("#943D53"));
        }

        // 成员
        findComponentById(ResourceTable.Id_schedule_dl_member).setClickedListener(component -> {
            Intent intent = new Intent();
            intent.setParam("memberIds", memberIds);
            present(new SelectMemberAbilitySlice(), intent);
        });
        memberName = (Text) findComponentById(ResourceTable.Id_schedule_text_member);
    }

    private void initSwitch() {
        ShapeElement elementThumbOn = new ShapeElement();
        elementThumbOn.setShape(ShapeElement.OVAL);
        elementThumbOn.setRgbColor(RgbColor.fromArgbInt(0xFFFFFFFF));
        elementThumbOn.setCornerRadius(50);
        // 关闭状态下滑块的样式
        ShapeElement elementThumbOff = new ShapeElement();
        elementThumbOff.setShape(ShapeElement.OVAL);
        elementThumbOff.setRgbColor(RgbColor.fromArgbInt(0xFFFFFFFF));
        elementThumbOff.setCornerRadius(50);
        // 开启状态下轨迹样式
        ShapeElement elementTrackOn = new ShapeElement();
        elementTrackOn.setShape(ShapeElement.RECTANGLE);
        elementTrackOn.setRgbColor(RgbColor.fromArgbInt(0xFF0A59F7));
        elementTrackOn.setCornerRadius(50);
        // 关闭状态下轨迹样式
        ShapeElement elementTrackOff = new ShapeElement();
        elementTrackOff.setShape(ShapeElement.RECTANGLE);
        elementTrackOff.setRgbColor(RgbColor.fromArgbInt(0xFF808080));
        elementTrackOff.setCornerRadius(50);

        timeSwitch.setTrackElement(trackElementInit(elementTrackOn, elementTrackOff));
        timeSwitch.setThumbElement(thumbElementInit(elementThumbOn, elementThumbOff));
    }

    private StateElement trackElementInit(ShapeElement on, ShapeElement off) {
        StateElement trackElement = new StateElement();
        trackElement.addState(new int[]{ComponentState.COMPONENT_STATE_CHECKED}, on);
        trackElement.addState(new int[]{ComponentState.COMPONENT_STATE_EMPTY}, off);
        return trackElement;
    }

    private StateElement thumbElementInit(ShapeElement on, ShapeElement off) {
        StateElement thumbElement = new StateElement();
        thumbElement.addState(new int[]{ComponentState.COMPONENT_STATE_CHECKED}, on);
        thumbElement.addState(new int[]{ComponentState.COMPONENT_STATE_EMPTY}, off);
        return thumbElement;
    }

    private void cancel(Component component) {
        terminateAbility();
    }

    private void confirm(Component component) {
        // 开始结束时间
        Text startTimeText = (Text) findComponentById(ResourceTable.Id_schedule_text_start);
        Text endTimeText = (Text) findComponentById(ResourceTable.Id_schedule_text_end);
        String startTime = scheduleDate + " " + startTimeText.getText();
        String endTime = scheduleDate + " " + endTimeText.getText();
        requestAddSchedule(startTime, endTime);
    }

    private void showLoadingDialog() {
        if (loadingDialog == null) {
            loadingDialog = new ACProgressFlower.Builder(this)
                    .text("正在创建日程中")
                    .build();
            loadingDialog.setCanceledOnTouchOutside(true);
        }
        loadingDialog.show(loadingDialog);
    }

    private void closeLoadingDialog() {
        if (loadingDialog != null) {
            loadingDialog.hide();
        }
    }

    /**
     * 请求添加日程
     */
    private void requestAddSchedule(String startTime, String endTime) {
        // 名字
        TextField scheduleName = (TextField) findComponentById(ResourceTable.Id_schedule_tf_name);
        String name = scheduleName.getText();
        // 日程命令
        JSONArray deviceConfigs = new JSONArray();
        commandMap.forEach((deviceId, command) -> {
            JSONObject deviceConfig = new JSONObject();
            deviceConfig.put("deviceId", deviceId);
            deviceConfig.put("command", command.getValue());
            deviceConfigs.add(deviceConfig);
        });

        if (StringUtils.isEmpty(name)) {
            XToastUtils.toast("请输入日程名字");
            return;
        } else if (deviceConfigs.isEmpty()) {
            XToastUtils.toast("请选择设备");
            return;
        } else if (StringUtils.isBlank(memberIds)) {
            XToastUtils.toast("请选择成员");
            return;
        }

        JSONObject body = new JSONObject();
        body.put("creatorId", PreferencesUtil.getUserId(getContext()));
        body.put("deviceConfig", deviceConfigs.toString());
        body.put("startTime", startTime);
        body.put("endTime", endTime);
        body.put("name", name);
        body.put("remindDay", StringUtils.join(getCheckedDay(multiSelectToggleGroup).toArray(), ","));
        body.put("sceneId", "sceneId");
        body.put("stateDetail", "");
        body.put("userIds", memberIds);
        KLog.i("body-body", body);
        showLoadingDialog();
        OKHttpUtilsRx2.INSTANCE.getApi().addSchedule(RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respond) {
                        closeLoadingDialog();
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            XToastUtils.toast("日程创建成功");
                            onBackPressed();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        closeLoadingDialog();
                        XToastUtils.toast("创建失败请重试(" + throwable.getMessage() + ")");
                    }
                });
    }

    private List<String> getScheduleDate(boolean isAllDay) {
        List<String> list = new ArrayList<>();
        Date now = new Date();
        String startString;
        String endString;
        if (isAllDay) {
            startString = "00:00:00";
            endString = "23:59:59";
        } else {
            SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss");
            Date startDate = new Date(now.getTime() + 2 * 60 * 1000);
            Date endDate = new Date(now.getTime() + 3 * 60 * 1000);
            startString = format.format(startDate);
            endString = format.format(endDate);
        }
        list.add(startString);
        list.add(endString);
        return list;
    }

    /**
     * 获取是第几个设备
     *
     * @param multiSelectToggleGroup multiSelectToggleGroup
     * @return Set
     */
    public Set<Integer> getCheckedDay(MultiSelectToggleGroup multiSelectToggleGroup) {
        Set<Integer> ids = new LinkedHashSet<>();
        for (int i = 0; i < multiSelectToggleGroup.getChildCount(); i++) {
            Component child = multiSelectToggleGroup.getComponentAt(i);
            if (child instanceof AbsButton && ((AbsButton) child).isChecked()
                    || child instanceof MarkerButton && ((MarkerButton) child).isChecked()) {
                ids.add(i + 1);
            }
        }
        return ids;
    }

    public void clearCheck(MultiSelectToggleGroup parentComponent) {
        for (int i = 0; i < parentComponent.getChildCount(); i++) {
            Component child = parentComponent.getComponentAt(i);
            if (child instanceof AbsButton && ((AbsButton) child).isChecked()) {
                ((AbsButton) child).setChecked(false);
            } else if (child instanceof MarkerButton && ((MarkerButton) child).isChecked()) {
                ((MarkerButton) child).setChecked(false);
            }
        }
    }

    public void setMaxSelectCount(MultiSelectToggleGroup multiSelectToggleGroup, int maxSelectCount) {
        if (maxSelectCount < 0) {
            return;
        }

        List<Component> uncheckedViews = new ArrayList<>();
        int checkedCount = 0;
        for (int i = 0; i < multiSelectToggleGroup.getChildCount(); i++) {
            Component view = multiSelectToggleGroup.getComponentAt(i);
            if (view instanceof MarkerButton) {
                if (((MarkerButton) view).isChecked()) {
                    checkedCount++;
                } else {
                    uncheckedViews.add(view);
                }
            }
        }
        if (checkedCount >= maxSelectCount) {
            for (Component view : uncheckedViews) {
                view.setEnabled(false);
            }
        } else {
            for (Component view : uncheckedViews) {
                view.setEnabled(true);
            }
        }
    }

    /**
     * 创建时间选择器
     *
     * @param text 需要修改文本的组件
     */
    private void createTimePicker(Text text) {
        CommonDialog commonDialog = new CommonDialog(getContext());
        Component dialog = LayoutScatter.getInstance(getContext())
                .parse(ResourceTable.Layout_dialog_time_pick, null, true);
        commonDialog.setSize(AttrHelper.vp2px(320, getContext()), AttrHelper.vp2px(264, getContext()));
        commonDialog.setContentCustomComponent(dialog);

        TimePicker timePicker = (TimePicker) dialog.findComponentById(ResourceTable.Id_dialog_tp_picker);
        timePicker.showSecond(false);
        dialog.findComponentById(ResourceTable.Id_dialog_btn_confirm).setClickedListener(component -> {
            updateTimeText(text, timePicker.getHour(), timePicker.getMinute(), 0);
            commonDialog.destroy();
        });
        dialog.findComponentById(ResourceTable.Id_dialog_btn_cancel).setClickedListener(component -> {
            commonDialog.destroy();
        });
        commonDialog.show();
    }

    private void updateTimeText(Text text, int hour, int minute, int second) {
        text.setText(formatTime(hour) + ":" + formatTime(minute) + ":" + formatTime(second));
    }

    private String formatTime(int time) {
        if (time < 10) {
            return "0" + time;
        } else {
            return "" + time;
        }
    }
}
