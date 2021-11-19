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

package com.example.distschedule.toast;

public class XToastUtils {
    static {
        XToast.Config.get()
                .setAlpha(200)
                .allowQueue(false);
    }

    private XToastUtils() {
        throw new UnsupportedOperationException("u can't instantiate me...");
    }

    public static void toast(String message) {
        XToast.normal(DistSchedule.getContext(), message).show();
    }

    public static void toast(int message) {
        XToast.normal(DistSchedule.getContext(), message).show();
    }

    public static void toast(String message, int duration) {
        XToast.normal(DistSchedule.getContext(), message, duration).show();
    }

    public static void toast(int message, int duration) {
        XToast.normal(DistSchedule.getContext(), message, duration).show();
    }

    //=============//
    public static void error(String message) {
        XToast.error(DistSchedule.getContext(), message).show();
    }

    public static void error(Exception error) {
        XToast.error(DistSchedule.getContext(), error.getMessage()).show();
    }

    public static void error(int message) {
        XToast.error(DistSchedule.getContext(), message).show();
    }

    public static void error(String message, int duration) {
        XToast.error(DistSchedule.getContext(), message, duration).show();
    }

    public static void error(int message, int duration) {
        XToast.error(DistSchedule.getContext(), message, duration).show();
    }

    //=============//
    public static void success(String message) {
        XToast.success(DistSchedule.getContext(), message).show();
    }

    public static void success(int message) {
        XToast.success(DistSchedule.getContext(), message).show();
    }

    public static void success(String message, int duration) {
        XToast.success(DistSchedule.getContext(), message, duration).show();
    }

    public static void success(int message, int duration) {
        XToast.success(DistSchedule.getContext(), message, duration).show();
    }

    //=============//
    public static void info(String message) {
        XToast.info(DistSchedule.getContext(), message).show();
    }

    public static void info(int message) {
        XToast.info(DistSchedule.getContext(), message).show();
    }

    public static void info(String message, int duration) {
        XToast.info(DistSchedule.getContext(), message, duration).show();
    }

    public static void info(int message, int duration) {
        XToast.info(DistSchedule.getContext(), message, duration).show();
    }

    //=============//

    public static void warning(String message) {
        XToast.warning(DistSchedule.getContext(), message).show();
    }

    public static void warning(int message) {
        XToast.warning(DistSchedule.getContext(), message).show();
    }

    public static void warning(String message, int duration) {
        XToast.warning(DistSchedule.getContext(), message, duration).show();
    }

    public static void warning(int message, int duration) {
        XToast.warning(DistSchedule.getContext(), message, duration).show();
    }

}
