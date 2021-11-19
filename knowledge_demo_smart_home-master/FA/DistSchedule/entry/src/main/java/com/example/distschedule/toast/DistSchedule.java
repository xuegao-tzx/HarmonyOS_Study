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

import ohos.aafwk.ability.AbilityPackage;

public class DistSchedule {
    private static AbilityPackage sContext;

    //=======================初始化设置===========================//

    /**
     * 初始化
     *
     * @param context 上下文
     */
    public static void init(AbilityPackage context) {
        sContext = context;
    }

    public static AbilityPackage getContext() {
        testInitialize();
        return sContext;
    }

    private static void testInitialize() {
        if (sContext == null) {
            throw new ExceptionInInitializerError("请先在全局Application中调用 DistSchedule.init() 初始化！");
        }
    }

}
