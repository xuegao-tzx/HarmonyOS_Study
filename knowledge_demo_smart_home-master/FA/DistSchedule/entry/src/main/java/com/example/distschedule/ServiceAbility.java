/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

package com.example.distschedule;

import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.ScheduleResult;
import com.example.distschedule.listener.DeviceStateListener;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.ReminderHelperUtils;
import com.socks.library.KLog;
import ohos.aafwk.ability.Ability;
import ohos.aafwk.content.Intent;
import ohos.app.Context;
import ohos.app.dispatcher.TaskDispatcher;
import ohos.app.dispatcher.task.TaskPriority;
import ohos.event.notification.NotificationRequest;
import ohos.event.notification.ReminderRequest;
import ohos.event.notification.ReminderRequestAlarm;
import ohos.miscservices.timeutility.Time;
import ohos.rpc.IRemoteObject;
import ohos.rpc.MessageOption;
import ohos.rpc.MessageParcel;
import ohos.rpc.RemoteObject;
import org.apache.commons.lang3.StringUtils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

public class ServiceAbility extends Ability {
    private static final String TAG = ServiceAbility.class.getSimpleName();
    private static final Map<String, Integer> idMap = new HashMap<>();

    @Override
    public void onStart(Intent intent) {
        KLog.i(TAG, "ServiceAbility::onStart");
        // 创建通知，其中1005为notificationId
        NotificationRequest request = new NotificationRequest(1005);
        NotificationRequest.NotificationNormalContent content = new NotificationRequest.NotificationNormalContent();
        content.setTitle("日程").setText("运行中");
        NotificationRequest.NotificationContent notificationContent = new NotificationRequest.NotificationContent(content);
        request.setContent(notificationContent);

        // 绑定通知，1005为创建通知时传入的notificationId
        keepBackgroundRunning(1005, request);

        ReminderHelperUtils.getInstance().cancelAllReminders();
        createScheduleNotice();
        createDeviceMsgListener();

        super.onStart(intent);
    }

    private void createDeviceMsgListener() {
        Context that = this;
        TaskDispatcher globalTaskDispatcher = getGlobalTaskDispatcher(TaskPriority.HIGH);
        globalTaskDispatcher.asyncDispatch(() -> {
            DeviceStateListener.getInstance().listen(that);//监听IOT消息
        });
    }

    private void createScheduleNotice() {

        TaskDispatcher globalTaskDispatcher = getGlobalTaskDispatcher(TaskPriority.DEFAULT);
        globalTaskDispatcher.asyncDispatch(() -> {
            while (true) {
                getTodaySchedule();
                Time.sleep(30 * 1000);
            }
        });
    }


    private void getTodaySchedule() {
        if (StringUtils.isEmpty(PreferencesUtil.getUserId(getContext()))) {
            return;
        }
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
        Date now = new Date();
        String today = format.format(now);
        String startTime = today + " 00:00:00";
        String endTime = today + " 23:59:59";
        OKHttpUtilsRx2.INSTANCE.getApi().getSchedule(PreferencesUtil.getUserId(getContext()), startTime, endTime)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<ScheduleResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<ScheduleResult>> respond) {
                        super.onNext(respond);
                        KLog.i(getString(ResourceTable.String_log_tag), JSONObject.toJSONString(respond));
                        List<ScheduleResult> list = new ArrayList<>();
                        if (!respond.isEmptyResult()) {
                            Calendar cal = Calendar.getInstance();
                            cal.setTime(now);
                            int dayOfWeek = cal.get(Calendar.DAY_OF_WEEK) - 1;
                            if (dayOfWeek == 0) {
                                dayOfWeek = 7;
                            }

                            for (ScheduleResult item : respond.getResult()) {
                                if ((StringUtils.contains(item.getRemindDay(), String.valueOf(dayOfWeek))
                                        || StringUtils.isEmpty(item.getRemindDay()))) {
                                    try {
                                        SimpleDateFormat scheduleFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                                        Date scheduleDate = scheduleFormat.parse(item.getStartTime());
                                        // 日程在当前时间之后
                                        if (scheduleDate.after(now)) {
                                            list.add(item);
                                        }
                                    } catch (ParseException e) {
                                        e.printStackTrace();
                                    }
                                }
                            }

                            setReminder(list);
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        // 可能没连上网请求失败，失败不弹框
                        KLog.e(TAG, throwable.getMessage());
                    }
                });
    }

    private void setReminder(List<ScheduleResult> list) {
        if (list.isEmpty()) {
            return;
        }
        list.forEach(scheduleResult -> {
            if (!idMap.containsKey(scheduleResult.getId())) {
                try {
                    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    Date scheduleDate = dateFormat.parse(scheduleResult.getStartTime());

                    Calendar calendar = Calendar.getInstance();
                    calendar.setTime(scheduleDate);
                    int hour = calendar.get(Calendar.HOUR_OF_DAY);
                    int time = calendar.get(Calendar.MINUTE);
                    int[] repeatDay = {};
                    ReminderRequest reminder = new ReminderRequestAlarm(hour, time, repeatDay);
                    reminder.setTitle("日程提醒")
                            .setContent(scheduleResult.getName())
                            .setRingDuration(3);
                    reminder.setIntentAgent(getBundleName(), MainAbility.class.getName());
                    int id = ReminderHelperUtils.getInstance().addReminder(reminder);
                    KLog.i("ReminderRequest-ReminderRequest", hour + time + " " + id);
                    idMap.put(scheduleResult.getId(), id);
                } catch (ParseException e) {
                    KLog.e(TAG, e.getMessage());
                    e.printStackTrace();
                }
            }
        });
    }

    public boolean test() {
        return true;
    }

    @Override
    public void onBackground() {
        super.onBackground();
        KLog.i(TAG, "ServiceAbility::onBackground");
    }

    @Override
    public void onStop() {
        cancelBackgroundRunning();
        super.onStop();
        KLog.i(TAG, "ServiceAbility::onStop");
    }

    @Override
    public void onCommand(Intent intent, boolean restart, int startId) {

    }

    @Override
    public IRemoteObject onConnect(Intent intent) {
        KLog.i("onConnect success");
        return new CurrentRemoteObject(this);
    }

    @Override
    public void onDisconnect(Intent intent) {
    }

    public static class CurrentRemoteObject extends RemoteObject {
        private final ServiceAbility serviceAbility;

        CurrentRemoteObject(ServiceAbility serviceAbility) {
            super("CurrentRemoteObject");
            this.serviceAbility = serviceAbility;
        }

        @Override
        public boolean onRemoteRequest(int code, MessageParcel data, MessageParcel reply, MessageOption option) {
            return true;
        }

        public boolean test() {
            return serviceAbility.test();
        }
    }
}