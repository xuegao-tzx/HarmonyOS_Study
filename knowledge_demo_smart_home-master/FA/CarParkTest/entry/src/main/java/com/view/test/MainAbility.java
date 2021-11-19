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

package com.view.test;

import com.view.test.slice.MainAbilitySlice;
import com.view.test.slice.SecondAbilitySlice;
import ohos.aafwk.ability.Ability;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Component;
import ohos.agp.utils.Color;
import ohos.agp.window.service.WindowManager;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.multimodalinput.event.KeyEvent;


public class MainAbility extends Ability {



    private static final HiLogLabel LABEL_LOG = new HiLogLabel(HiLog.LOG_APP, 0x11101, "MainAbility");

    @Override
    public void onStart(Intent intent) {
        getWindow().setStatusBarColor(Color.WHITE.getValue());
        getWindow().setStatusBarVisibility(Component.VISIBLE);
        getWindow().addFlags(WindowManager.LayoutConfig.MARK_TRANSLUCENT_STATUS |
                WindowManager.LayoutConfig.MARK_TRANSLUCENT_NAVIGATION
        );
        super.onStart(intent);
        super.setMainRoute(MainAbilitySlice.class.getName());

        addActionRoute("action.second", SecondAbilitySlice.class.getName());

        HiLog.error(LABEL_LOG, "onStart");
    }

    @Override
    protected void onActive() {
        super.onActive();
        HiLog.info(LABEL_LOG, "onActive");
    }

    @Override
    protected void onInactive() {
        super.onInactive();
        HiLog.info(LABEL_LOG, "onInactive");
    }

    @Override
    protected void onBackground() {
        super.onBackground();
        HiLog.info(LABEL_LOG, "onBackground");
    }

    @Override
    protected void onForeground(Intent intent) {
        super.onForeground(intent);
        HiLog.info(LABEL_LOG, "onForeground");
    }

    @Override
    protected void onStop() {
        super.onStop();
        HiLog.info(LABEL_LOG, "onStop");
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent keyEvent) {
        HiLog.info(LABEL_LOG, "--------------------------onKeyDown");
        return super.onKeyDown(keyCode, keyEvent);
    }
}
