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

import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.DeviceResult;
import ohos.agp.components.Component;
import ohos.agp.components.DirectionalLayout;
import ohos.agp.components.Image;
import ohos.agp.components.Text;
import ohos.agp.utils.LayoutAlignment;
import ohos.app.Context;

import java.util.ArrayList;
import java.util.List;

public class DeviceListPanel {

    public static Component getDeviceCardById(Context context, DeviceResult deviceList) {

        DirectionalLayout deviceCard = new DirectionalLayout(context);
        deviceCard.setAlignment(LayoutAlignment.HORIZONTAL_CENTER);
        deviceCard.setComponentSize(500, 400);

        Image deviceLogo = new Image(context);
        deviceLogo.setPixelMap(ResourceTable.Media_icon_more_device);

        DirectionalLayout secondLay = new DirectionalLayout(context);
        secondLay.setAlignment(LayoutAlignment.VERTICAL_CENTER);

        Text deviceName = new Text(context);
        deviceName.setText(deviceList.getName());

        Text deviceOwner = new Text(context);
        deviceOwner.setText(deviceList.getOwnerId());

        secondLay.addComponent(deviceName);
        secondLay.addComponent(deviceOwner);

        deviceCard.addComponent(deviceLogo);
        deviceCard.addComponent(secondLay);

        return deviceCard;
    }


    public static Component getDeviceListPanel(Context context, List<DeviceResult> deviceList) {
        DirectionalLayout devicePanel = new DirectionalLayout(context);
        devicePanel.setAlignment(LayoutAlignment.VERTICAL_CENTER);
        boolean addType1 = false;
        boolean addType2 = false;

        DeviceResult dr1 = new DeviceResult();
        dr1.setName("手表");
        dr1.setOwnerId("来自儿子的共享");
        dr1.setTypeId(1);
        DeviceResult dr2 = new DeviceResult();
        dr1.setName("扫地机器人");
        dr1.setOwnerId("来自老婆的共享");
        dr1.setTypeId(2);
        List<DeviceResult> list = new ArrayList<DeviceResult>();
        list.add(dr1);
        list.add(dr2);

        for (DeviceResult dr : deviceList) {

            if (null == dr) {
                return devicePanel;
            }
            int type = dr.getTypeId();
            switch (type) {
                case 1:
                    if (!addType1) {
                        Text typeName1 = new Text(context);
                        typeName1.setText("随身设备");
                        addType1 = true;
                        devicePanel.addComponent(typeName1);
                    }
                    break;
                case 2:
                    if (!addType2) {
                        Text typeName2 = new Text(context);
                        typeName2.setText("家务设备");
                        addType2 = true;
                        devicePanel.addComponent(typeName2);
                    }
                    break;
            }

            devicePanel.addComponent(getDeviceCardById(context, dr));

        }
        return devicePanel;
    }
}
