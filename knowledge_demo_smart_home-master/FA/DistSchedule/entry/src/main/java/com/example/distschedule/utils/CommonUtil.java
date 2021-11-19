/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
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

package com.example.distschedule.utils;

import com.example.distschedule.DeviceControlAbility;
import com.example.distschedule.database.DeviceData;
import com.example.distschedule.database.DeviceDataBean;
import com.example.distschedule.toast.XToastUtils;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.Component;
import ohos.agp.window.dialog.ToastDialog;
import ohos.app.Context;

import java.util.Map;

/**
 * Common util
 *
 * @since 2021-08-21
 */
public class CommonUtil {
    private static final String TAG_LOG = "CommonUtil";

    private CommonUtil() {
    }

    /**
     * show
     *
     * @param context context
     * @param msg     message
     */
    public static void showUiTip(Context context, String msg) {
        context.getUITaskDispatcher().delayDispatch(() -> {
            ToastDialog toastDialog = new ToastDialog(context);
            toastDialog.setAutoClosable(false);
            toastDialog.setContentText(msg);
            toastDialog.show();
        }, 0);
    }

    /**
     * 获取图片名称
     *
     * @param component 组件
     * @return 图片名称
     */
    public static String getImageName(Component component) {
        String imageName = "";
        if (component.getTag() instanceof String) {
            imageName = (String) component.getTag();
        }
        return imageName;
    }

    public static void judgeDevice(Context context, String deviceId, Intent intent) {
        if (intent == null) {
            intent = new Intent();
        }
        intent.setParam("deviceId", deviceId);
        try {
            Map<String, DeviceDataBean> map = DeviceData.INSTANCE.getDeviceData();
            String productId = deviceId.split("_")[0];
            toControlDevice(context, map.get(productId).getAction(), intent);
        } catch (Exception e) {
            XToastUtils.warning("跳转失败错误");
        }
    }

    private static void toControlDevice(Context context, String action, Intent intent) {
        Operation operation = new Intent.OperationBuilder()
                .withBundleName(context.getBundleName())
                .withAbilityName(DeviceControlAbility.class.getName())
                .withAction(action)
                .build();
        intent.setOperation(operation);
        context.startAbility(intent, 0);
    }

    /**
     * Num 数字常量类
     *
     * @since 2021-05-10
     */
    public static class Num {
        /**
         * read bytes num
         */
        public static final int READ_BYTES_NUM = -1;

        /**
         * bytes num
         */
        public static final int BYTES_NUM = 1024;

        /**
         * two
         */
        public static final int TWO = 2;

        /**
         * three
         */
        public static final int THREE = 3;

        /**
         * four
         */
        public static final int FOUR = 4;

        /**
         * six
         */
        public static final int SIX = 6;

        /**
         * eight
         */
        public static final int EIGHT = 8;

        /**
         * sixteen
         */
        public static final int SIXTEEN = 16;
    }
}
