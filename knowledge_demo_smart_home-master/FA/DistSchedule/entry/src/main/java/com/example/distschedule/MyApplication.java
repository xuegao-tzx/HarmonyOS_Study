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

import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.DistSchedule;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilityPackage;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import org.apache.commons.lang3.StringUtils;
import org.greenrobot.eventbus.EventBus;
import ren.yale.ohos.retrofitcachelib.RetrofitCache;

public class MyApplication extends AbilityPackage {
    @Override
    public void onInitialize() {
        KLog.init(true, "DistSchedule_log");
        OKHttpUtilsRx2.INSTANCE.init(this);
        RetrofitCache.getInstance().init(this).enableMock(true);
        checkLogin();
        super.onInitialize();
        EventBus.initThreadForHos(getUITaskDispatcher());
        DistSchedule.init(this);
    }

    @Override
    public void onEnd() {
        super.onEnd();
    }

    private void checkLogin() {
        if (StringUtils.isBlank(PreferencesUtil.getUserId(getApplicationContext()))) {
            Intent loginIntent = new Intent();
            Operation operation = new Intent.OperationBuilder()
                    .withBundleName(getBundleName())
                    .withAbilityName(LoginAbility.class.getName())
                    .withFlags(Intent.FLAG_ABILITY_NEW_MISSION)
                    .build();
            loginIntent.setOperation(operation);
            startAbility(loginIntent, 0);
        }
    }

}
