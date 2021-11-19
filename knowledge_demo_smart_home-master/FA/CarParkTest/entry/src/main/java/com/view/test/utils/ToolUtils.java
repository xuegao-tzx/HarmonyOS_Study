/*
 * Copyright (c) 2021 KaiHong Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.view.test.utils;

import ohos.data.distributed.common.Value;
import ohos.data.preferences.Preferences;

public class ToolUtils {
    public static String getDateStr(String str) {
        if (null == str) {
            return null;
        }

        StringBuilder sb = new StringBuilder(str);
        sb.replace(8,9, " ");
        sb.replace(15, 16, "");


        sb.insert(4, "-");
        sb.insert(7, "-");
        sb.insert(13, ":");
        sb.insert(16, ":");

        return sb.toString();
    }

    public static void writeToSP(Preferences pf, String key, String value) {
        if (null != pf) {
            pf.putString(key, value);
            pf.flush();
        }
    }
}
