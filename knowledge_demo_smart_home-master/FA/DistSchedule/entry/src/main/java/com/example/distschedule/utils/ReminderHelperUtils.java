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

package com.example.distschedule.utils;

import com.socks.library.KLog;
import ohos.event.notification.*;
import ohos.rpc.RemoteException;

/**
 * 通知工具类
 * 参考官网：
 * https://developer.harmonyos.com/cn/docs/documentation/doc-guides/ability-notification-fwk-scheduled-reminders-0000001123755782
 * <p>
 */
public class ReminderHelperUtils {

    public static final String defaultSlotId = "defaultSlotId";
    public static final String defaultSlotName = "defaultSlotName";
    private static final String TAG = ReminderHelperUtils.class.getSimpleName();
    private static ReminderHelperUtils reminderHelperUtils;

    private ReminderHelperUtils() {
        // 1. 设置渠道信息
        NotificationSlot defaultSlot = new NotificationSlot(defaultSlotId, defaultSlotName, NotificationSlot.LEVEL_HIGH);
        defaultSlot.setEnableLight(true);
        defaultSlot.setEnableVibration(true);
        defaultSlot.enableBadge(true);
        defaultSlot.setLockscreenVisibleness(NotificationRequest.VISIBLENESS_TYPE_PUBLIC);
        // 2. 向代理服务添加渠道对象
        try {
            ReminderHelper.addNotificationSlot(defaultSlot);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

    }

    /**
     * 获取一个通知工具类实例
     *
     * @return 返回实例
     */
    public static ReminderHelperUtils getInstance() {
        if (null == reminderHelperUtils) {
            reminderHelperUtils = new ReminderHelperUtils();
        }
        return reminderHelperUtils;
    }

    /**
     * 添加一个通知
     *
     * @param reminderRequest reminderRequest
     * @return 通知ID
     */
    public int addReminder(ReminderRequest reminderRequest) {
        int reminderId = 0;
        try {
            reminderId = ReminderHelper.publishReminder(reminderRequest);
        } catch (ReminderManager.AppLimitExceedsException | ReminderManager.SysLimitExceedsException | RemoteException e) {
            KLog.e(TAG, e.getMessage());
            e.printStackTrace();
        }
        return reminderId;
    }

    /**
     * 取消一个通知
     *
     * @param reminderId reminderId
     */
    public void removeReminder(int reminderId) {
        try {
            ReminderHelper.cancelReminder(reminderId);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * 取消所有通知
     */
    public void cancelAllReminders() {
        try {
            ReminderHelper.cancelAllReminders();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public int addReminder(NotificationSlot slot, ReminderRequest rr) {
        try {
            ReminderHelper.addNotificationSlot(slot);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        int reminderId = 0;
        try {
            reminderId = ReminderHelper.publishReminder(rr);
        } catch (ReminderManager.AppLimitExceedsException | ReminderManager.SysLimitExceedsException | RemoteException e) {
            e.printStackTrace();
        }
        return reminderId;
    }

}
