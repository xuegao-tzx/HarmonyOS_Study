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

package com.example.distschedule.provider;

import com.example.distschedule.DeviceControlAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.database.AlarmBean;
import com.example.distschedule.database.DeviceData;
import com.example.distschedule.database.DeviceDataBean;
import com.example.distschedule.toast.XToastUtils;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.*;
import ohos.agp.utils.Color;

import java.util.List;
import java.util.Map;

public class AlarmListProvider extends BaseItemProvider {
    private final List<AlarmBean> list;
    private final AbilitySlice slice;

    public AlarmListProvider(AbilitySlice slice, List<AlarmBean> list) {
        this.list = list;
        this.slice = slice;
    }

    @Override
    public int getCount() {
        return list == null ? 0 : list.size();
    }

    @Override
    public Object getItem(int position) {
        if (list != null && position >= 0 && position < list.size()) {
            return list.get(position);
        }
        return null;
    }

    @Override
    public long getItemId(int position) {
        //可添加具体处理逻辑
        return position;
    }

    @Override
    public Component getComponent(int position, Component convertComponent, ComponentContainer componentContainer) {
        final Component cpt;
        if (convertComponent == null) {
            cpt = LayoutScatter.getInstance(slice).parse(ResourceTable.Layout_item_alarm, null, false);
        } else {
            cpt = convertComponent;
        }
        AlarmBean item = list.get(position);
        Text typeText = (Text) cpt.findComponentById(ResourceTable.Id_alarm_text_type);
        typeText.setText(item.getTypeValue() + "  " + item.getTime());
        ((Text) cpt.findComponentById(ResourceTable.Id_alarm_text_message)).setText(item.getMessage());
        if (!item.isCriticalAlarm()) {
            typeText.setTextColor(new Color(slice.getColor(ResourceTable.Color_black)));
            ((Image) cpt.findComponentById(ResourceTable.Id_alarm_img_icon)).setPixelMap(ResourceTable.Media_icon_notice);
        }

        String deviceId = item.getDeviceId();
        cpt.findComponentById(ResourceTable.Id_alarm_dl_container).setClickedListener(component -> {
            try {
                Map<String, DeviceDataBean> map = DeviceData.INSTANCE.getDeviceData();
                String productId = deviceId.split("_")[0];
                toControlDevice(deviceId, map.get(productId).getAction());
            } catch (Exception e) {
                XToastUtils.warning("跳转失败错误");
            }
        });
        return cpt;
    }

    private void toControlDevice(String deviceId, String action) {
        Intent intent = new Intent();
        intent.setParam("deviceId", deviceId);
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(slice.getBundleName())
                        .withAbilityName(DeviceControlAbility.class.getName())
                        .withAction(action)
                        .build();
        intent.setOperation(operation);
        slice.startAbility(intent);
    }
}
