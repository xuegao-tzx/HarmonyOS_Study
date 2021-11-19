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

import com.example.distschedule.ResourceTable;
import com.example.distschedule.event.ChangeTabEvent;
import com.example.distschedule.fraction.MainFraction;
import com.example.distschedule.fraction.MyFraction;
import com.example.distschedule.fraction.ScheduleFraction;
import com.example.distschedule.utils.FractionUtil;
import com.example.distschedule.utils.QRCode;
import com.socks.library.KLog;
import me.majiajie.pagerbottomtabstrip.NavigationController;
import me.majiajie.pagerbottomtabstrip.PageNavigationView;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.ability.fraction.Fraction;
import ohos.aafwk.content.Intent;
import ohos.agp.utils.Color;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

/**
 * 包含底部导航栏的页面，该页面跳转需要使用fraction切换，当前页面编写需要使用fraction进行编写
 *
 * @since 2021-08-28
 */
public class MainAbilitySlice extends AbilitySlice {
    public static QRCode qrCode;
    private Fraction sceneFraction;
    private Fraction scheduleFraction;
    private Fraction myFraction;
    private NavigationController navController;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_main);
        EventBus.getDefault().register(this);
        initComponents();
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onShowMessageEvent(ChangeTabEvent messageEvent) {
        if (messageEvent.getChangeTag() == 2) {
            navController.setSelect(1);
        }
    }

    private void initComponents() {
        if (null == qrCode) {
            qrCode = new QRCode(this);
        }

        if (sceneFraction == null) {
            sceneFraction = new MainFraction();
        }
        if (scheduleFraction == null) {
            scheduleFraction = new ScheduleFraction();
        }
        if (myFraction == null) {
            myFraction = new MyFraction();
        }

        FractionUtil.replaceFraction(getAbility(), ResourceTable.Id_main_dl_container, sceneFraction);
        // 底部导航栏
        PageNavigationView tab = (PageNavigationView) findComponentById(ResourceTable.Id_main_nav_navigation);
        navController = tab.material()
                .addItem(ResourceTable.Media_icon_home_selected, "主页", Color.getIntColor("#0A59F7"))
                .addItem(ResourceTable.Media_icon_calender_selected, "日程", Color.getIntColor("#0A59F7"))
                .addItem(ResourceTable.Media_icon_mine_selected, "我的", Color.getIntColor("#0A59F7"))
                .setDefaultColor(0x000000) // 未选中状态的颜色
                .build();
        if (navController != null) {
            navController.addSimpleTabItemSelectedListener((index, old) -> {
                KLog.i("mNavigationController index:" + index);
                // 选中时触发
                switch (index) {
                    case 0: {
                        FractionUtil.replaceFraction(getAbility(), ResourceTable.Id_main_dl_container, sceneFraction);
                        break;
                    }
                    case 1: {
                        FractionUtil.replaceFraction(getAbility(), ResourceTable.Id_main_dl_container, scheduleFraction);
                        break;
                    }
                    case 2: {
                        FractionUtil.replaceFraction(getAbility(), ResourceTable.Id_main_dl_container, myFraction);
                        break;
                    }
                }
            });
        }
    }

    @Override
    protected void onBackground() {
        super.onBackground();
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    @Override
    protected void onBackPressed() {
        if (FractionUtil.getFractionByTag(getAbility(), MainFraction.class.getSimpleName()).isPresent() ||
                FractionUtil.getFractionByTag(getAbility(), MyFraction.class.getSimpleName()).isPresent() ||
                FractionUtil.getFractionByTag(getAbility(), ScheduleFraction.class.getSimpleName()).isPresent()) {
            FractionUtil.popFraction(getAbility());
        } else {
            super.onBackPressed();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }
}
