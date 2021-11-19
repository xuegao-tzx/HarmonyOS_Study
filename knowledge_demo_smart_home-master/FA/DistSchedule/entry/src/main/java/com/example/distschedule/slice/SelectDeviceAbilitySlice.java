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

import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.Command;
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.DevicesType;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.event.MessageEvent;
import com.example.distschedule.provider.SelectDeviceProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 * 选择设备
 *
 * @since 2021-08-28
 */
public class SelectDeviceAbilitySlice extends AbilitySlice implements Component.ClickedListener {
    public static final String KEY_IS_SET_COMMAND = "Is_Set_Command";
    private static final List<DeviceResult> listDevices = new ArrayList<>();
    private static final List<DeviceResult> listCategpry1 = new ArrayList<>();
    private static final List<DeviceResult> listCategpry2 = new ArrayList<>();
    public static Map<Integer, Integer> typeIdAndCategory = new HashMap<>();
    public Map<String, Command> commandMap = new HashMap<>();
    private SelectDeviceProvider provider;
    //选择的设备名称
    private String selectDeviceName = "";
    private Text textEmpty;


    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_select_device);
        EventBus.getDefault().register(this);
        initComponents();
    }

    @Override
    protected void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }


    private void initComponents() {
        selectDeviceName = "";
        Image imgCancel = (Image) findComponentById(ResourceTable.Id_select_device_img_cancel);
        Image imgConfirm = (Image) findComponentById(ResourceTable.Id_select_device_img_confirm);
        textEmpty = (Text) findComponentById(ResourceTable.Id_selectDevice_text_empty);
        ListContainer listContainer = (ListContainer) findComponentById(ResourceTable.Id_selectDevice_list_device);
        TableLayoutManager tableLayoutManager = new TableLayoutManager();
        tableLayoutManager.setColumnCount(1);
        listContainer.setLayoutManager(tableLayoutManager);
        provider = new SelectDeviceProvider(this);
        listContainer.setItemProvider(provider);
        provider.setOnSelectDeviceListener(new SelectDeviceProvider.OnSelectDeviceListener() {
            @Override
            public void onSelectDeviceClick(int position, boolean isChecked) {
                DeviceResult device = listDevices.get(position);
                String tempName = device.getName() + "、";

                String tempId = device.getId() + ",";
                if (isChecked) {
                    selectDeviceName = selectDeviceName + tempName;
                } else {
                    deleteCommand(device.getId());
                    selectDeviceName = selectDeviceName.replace(tempName, "");
                }
            }
        });

        imgCancel.setClickedListener(this);
        imgConfirm.setClickedListener(this);
        getDeviceList();
    }

    private void deleteCommand(String deviceId) {
        commandMap.keySet().removeIf(deviceId::equals);
    }

    private void initDeviceType() {
        if (typeIdAndCategory != null && !typeIdAndCategory.isEmpty()) {
            return;
        }
        List<DevicesType> devicesTypeList = PreferencesUtil.getDeviceType(getContext());
        typeIdAndCategory = new HashMap<>();
        devicesTypeList.forEach(item -> typeIdAndCategory.put(item.getId(), item.getCategory()));
    }


    private void getDeviceList() {
        initDeviceType();
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceListByUserId(PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<DeviceResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<DeviceResult>> deviceList) {
                        super.onNext(deviceList);
                        listDevices.clear();
                        listCategpry1.clear();
                        listCategpry2.clear();
                        if (deviceList.isSuccess() && deviceList.getResult() != null) {
                            for (int i = 0; i < deviceList.getResult().size(); i++) {
                                DeviceResult deviceResult = deviceList.getResult().get(i);
                                int category;
                                try {
                                    category = typeIdAndCategory.get(deviceResult.getTypeId());
                                } catch (Exception e) {
                                    XToastUtils.warning("category数值出错");
                                    return;
                                }
                                if (category == 1 || category == 2) {

                                    listCategpry1.add(deviceResult);
                                } else if (category == 2) {
                                    listCategpry2.add(deviceResult);
                                }

                            }
                            if (listCategpry1.size() > 0) {
                                listCategpry1.get(0).setDeviceType("家务设备");
                            }
                            if (listCategpry2.size() > 0) {
                                listCategpry2.get(0).setDeviceType("娱乐设备");
                            }
                            listDevices.addAll(listCategpry1);
                            listDevices.addAll(listCategpry2);
                            if (listDevices == null || listDevices.size() == 0) {
                                textEmpty.setVisibility(Component.VISIBLE);
                            } else {
                                textEmpty.setVisibility(Component.HIDE);
                            }
                            provider.setData(listDevices);
                        }

                    }
                });
    }


    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onShowMessageEvent(MessageEvent messageEvent) {
        KLog.i(messageEvent);
        if (MessageEvent.EVENT_GET_COMMAND.equals(messageEvent.getKey())) {
            commandMap.put(messageEvent.getDeviceId(), messageEvent.getCommand());
        }
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_select_device_img_cancel:
                terminateAbility();
                break;
            case ResourceTable.Id_select_device_img_confirm:
                if (provider.getSelectDevice() != null && provider.getSelectDevice().size() > 0) {
                    JSONObject jsonObject = new JSONObject();
                    jsonObject.put("commandMap", commandMap);
                    jsonObject.put("deviceNames", selectDeviceName);
                    EventBus.getDefault().post(new MessageEvent(MessageEvent.EVENT_GET_DEVICE_JSON, jsonObject));
                    terminate();
                } else {
                    XToastUtils.toast("未选中任何设备");
                }
                break;
            default:
                break;
        }
    }
}
