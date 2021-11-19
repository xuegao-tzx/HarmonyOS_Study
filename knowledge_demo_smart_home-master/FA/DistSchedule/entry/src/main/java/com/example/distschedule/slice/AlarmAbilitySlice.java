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
import com.example.distschedule.database.AlarmBean;
import com.example.distschedule.database.Dbhelper;
import com.example.distschedule.provider.AlarmListProvider;
import com.example.distschedule.utils.PreferencesUtil;
import com.j256.ormlite.dao.Dao;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.ListContainer;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

public class AlarmAbilitySlice extends AbilitySlice {
    List<AlarmBean> alarmList = new ArrayList<>();

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_alarm_ability);
        initData();
        initComponents();
    }

    @Override
    public void onStop() {
        super.onStop();
    }

    private void initData() {
        try {
            Dao alarmDao = Dbhelper.getInstance(getContext()).getDao(AlarmBean.class);
            alarmList = alarmDao.queryForEq(AlarmBean.COLUMN_USER_ID, PreferencesUtil.getUserId(getContext()));
        } catch (SQLException exception) {
            exception.printStackTrace();
        }
    }

    private void initComponents() {
        initAlarmList();
    }

    private void initAlarmList() {
        ListContainer listContainer = (ListContainer) findComponentById(ResourceTable.Id_alarm_lc_list);
        AlarmListProvider provider = new AlarmListProvider(this, alarmList);
        listContainer.setItemProvider(provider);
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }
}
