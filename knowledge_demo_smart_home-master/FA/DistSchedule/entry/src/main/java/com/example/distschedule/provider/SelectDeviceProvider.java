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
import com.example.distschedule.database.DeviceData;
import com.example.distschedule.database.DeviceDataBean;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.CommonUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.Util;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.*;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.utils.Color;
import org.apache.commons.lang3.StringUtils;

import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class SelectDeviceProvider extends BaseItemProvider {
    public static final String KEY_IS_SET_COMMAND = "Is_Set_Command";
    private List<DeviceResult> list;
    private AbilitySlice slice;
    private Set<DeviceResult> selectDevice;
    private boolean isAllSelected = true;
    private OnSelectDeviceListener onSelectDeviceListener;

    public SelectDeviceProvider(AbilitySlice slice) {
        this.slice = slice;
    }

    public void setOnSelectDeviceListener(OnSelectDeviceListener onSelectDeviceListener) {
        this.onSelectDeviceListener = onSelectDeviceListener;
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
            cpt = LayoutScatter.getInstance(slice).parse(ResourceTable.Layout_item_select_device, null, false);
        } else {
            cpt = convertComponent;
        }

        Image icon = (Image) cpt.findComponentById(ResourceTable.Id_selectDevice_img_icon);
        Text deviceName = (Text) cpt.findComponentById(ResourceTable.Id_selectDevice_text_deviceName);
        Text deviceOwner = (Text) cpt.findComponentById(ResourceTable.Id_selectDevice_text_deviceOwner);
        Text isOnline = (Text) cpt.findComponentById(ResourceTable.Id_selectDevice_text_isOnLine);
        Text deviceType = (Text) cpt.findComponentById(ResourceTable.Id_selectDevice_text_deviceType);
        Image checkbox = (Image) cpt.findComponentById(ResourceTable.Id_member_img_checkbox);
        DeviceResult deviceResult = list.get(position);
        deviceName.setText(deviceResult.getName());
        if (deviceResult.getOwnerName().equals(PreferencesUtil.getUserName(slice))) {
            deviceOwner.setText("来自我的设备");
        } else {
            deviceOwner.setText("来自" + deviceResult.getOwnerName() + "的设备");
        }

        if (StringUtils.isBlank(deviceResult.getDeviceType())) {
            deviceType.setVisibility(Component.HIDE);
        } else {
            deviceType.setVisibility(Component.VISIBLE);
            deviceType.setText(deviceResult.getDeviceType());
        }

        if (deviceResult.isOnline() || deviceResult.getTypeId() == 5 || deviceResult.getTypeId() == 6 || deviceResult.getTypeId() == 7) {
            deviceName.setTextColor(new Color(Color.getIntColor("#000000")));
            try {
                Map<String, DeviceDataBean> map = DeviceData.INSTANCE.getDeviceData();
                String productId = deviceResult.getId().split("_")[0];
                icon.setPixelMap(map.get(productId).getOnlineImage());
            } catch (Exception e) {
                XToastUtils.warning("设备离线状态图片加载错误");
            }
            isOnline.setTextColor(new Color(Color.getIntColor("#0A59F7")));
            ShapeElement elementOnline = new ShapeElement();
            elementOnline.setShape(ShapeElement.RECTANGLE);
            float[] radii = {0, 0, Util.vp2px(slice.getContext(), 8), Util.vp2px(slice.getContext(), 8), 0, 0, Util.vp2px(slice.getContext(), 8), Util.vp2px(slice.getContext(), 8)};
            elementOnline.setCornerRadiiArray(radii);
            elementOnline.setRgbColor(new RgbColor(38, 110, 251, 25));
            isOnline.setBackground(elementOnline);
            isOnline.setText("在线");

        } else {
            try {
                Map<String, DeviceDataBean> map = DeviceData.INSTANCE.getDeviceData();
                String productId = deviceResult.getId().split("_")[0];
                icon.setPixelMap(map.get(productId).getOfflineImage());
            } catch (Exception e) {
                XToastUtils.warning("设备离线状态图片加载错误");
            }
            deviceName.setTextColor(new Color(Color.getIntColor("#cccccc")));
            isOnline.setTextColor(new Color(Color.getIntColor("#cccccc")));
            ShapeElement elementOffline = new ShapeElement();
            elementOffline.setShape(ShapeElement.RECTANGLE);
            float[] radii = {0, 0, Util.vp2px(slice.getContext(), 8), Util.vp2px(slice.getContext(), 8), 0, 0, Util.vp2px(slice.getContext(), 8), Util.vp2px(slice.getContext(), 8)};
            elementOffline.setCornerRadiiArray(radii);
            elementOffline.setRgbColor(new RgbColor(0, 0, 0, 13));
            isOnline.setBackground(elementOffline);
            isOnline.setText("离线");
        }

        if (onSelectDeviceListener != null) {
            cpt.findComponentById(ResourceTable.Id_selectDevice_dl_item).setClickedListener(component -> {
                if (deviceResult.isOnline()) {
                    Boolean isCheck = selectDevice.contains(deviceResult);
                    if (isCheck == null || !isCheck) {
                        selectDevice.add(deviceResult);
                        checkbox.setPixelMap(ResourceTable.Media_icon_selected);
                        onSelectDeviceListener.onSelectDeviceClick(position, true);
                        toDevicePage(deviceResult);
                    } else {
                        selectDevice.remove(deviceResult);
                        checkbox.setPixelMap(ResourceTable.Media_icon_unselected);
                        onSelectDeviceListener.onSelectDeviceClick(position, false);
                    }

                } else {
                    XToastUtils.toast("设备不在线");
                }

            });
        }

        return cpt;
    }

    private void toDevicePage(DeviceResult selectDevice) {
        Intent intent = new Intent();
        intent.setParam(KEY_IS_SET_COMMAND, true);
        CommonUtil.judgeDevice(slice.getContext(), selectDevice.getId(), intent);
    }

    public void setData(List<DeviceResult> list) {
        this.list = list;
        selectDevice = new HashSet<>(list.size());
        if (list != null) {
            notifyDataSetItemRangeChanged(0, list.size());
        }
    }

    public Set<DeviceResult> getSelectDevice() {
        if (selectDevice == null || selectDevice.size() == 0) {
            return null;
        }
        return selectDevice;
    }

    public void setIsAllSelected(boolean isAllSelected) {
        this.isAllSelected = isAllSelected;
        if (list != null) {
            notifyDataSetItemRangeChanged(0, list.size());
        }
    }

    public interface OnSelectDeviceListener {
        void onSelectDeviceClick(int position, boolean isChecked);
    }
}
