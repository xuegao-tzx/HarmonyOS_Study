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

package com.example.distschedule.utils;

import ohos.agp.components.AttrHelper;
import ohos.agp.window.service.DisplayManager;
import ohos.app.Context;

/**
 * 工具类
 *
 * @since 2021-09-08
 */
public class Util {
    private Util() {
    }

    /**
     * vp2px
     *
     * @param context context
     * @param vp      vp
     * @return int
     */
    public static int vp2px(Context context, float vp) {
        return AttrHelper.vp2px(vp, context);
    }

    /**
     * vp2px
     *
     * @param context context
     * @param fp      fp
     * @return int
     */
    public static int fp2px(Context context, float fp) {
        return AttrHelper.fp2px(fp, context);
    }

    /**
     * getScreenWidth
     *
     * @param context context
     * @return width
     */
    public static int getScreenWidth(Context context) {
        return DisplayManager.getInstance().getDefaultDisplay(context).get().getAttributes().width;
    }

    /**
     * getScreenHeight
     *
     * @param context context
     * @return height
     */
    public static int getScreenHeight(Context context) {
        return DisplayManager.getInstance().getDefaultDisplay(context).get().getAttributes().height;
    }
}
