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

package com.example.distschedule.fraction;

import com.example.distschedule.ResourceTable;
import com.example.distschedule.ScanAbility;
import com.example.distschedule.ScheduleManageAbility;
import com.example.distschedule.UpdateDeviceNameAbility;
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.DevicesType;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.database.DeviceData;
import com.example.distschedule.database.DeviceDataBean;
import com.example.distschedule.event.ChangeTabEvent;
import com.example.distschedule.event.MessageEvent;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.CommonUtil;
import com.example.distschedule.utils.FractionUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.Util;
import com.socks.library.KLog;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.*;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.utils.Color;
import ohos.agp.utils.LayoutAlignment;
import ohos.agp.utils.TextAlignment;
import ohos.agp.window.dialog.CommonDialog;
import org.apache.commons.lang3.StringUtils;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.*;

/**
 * 主页
 *
 * @since 2021-08-28
 */
public class MainFraction extends FractionBase {
    private static final String TAG = MainFraction.class.getName();
    public static Map<Integer, Integer> typeIdAndCategory = new HashMap<>();
    private final List<DeviceResult> portableDevices = new ArrayList<>();
    private final List<DeviceResult> homeDevices = new ArrayList<>();
    private final List<DeviceResult> entertainDevices = new ArrayList<>();
    private final List<DeviceResult> monitoringDevices = new ArrayList<>();
    private TabList tabType;
    private Text homeName;
    private StackLayout tabSense;
    private ScrollView deviceSv;
    private DirectionalLayout deviceListLayout;
    // 随身设备列表
    private TableLayout portableEquipmentLayout;
    private Text portableEquipment;
    // 家务设备列表
    private TableLayout homeEquipmentLayout;
    private Text homeEquipment;
    // 娱乐设备
    private TableLayout entertainEquipmentLayout;
    private Text entertainEquipment;
    // 监控设备
    private TableLayout monitoringEquipmentLayout;
    private Text monitoringEquipment;
    private String shareDeviceId = "";

    @Override
    protected Component onComponentAttached(LayoutScatter scatter, ComponentContainer container, Intent intent) {
        scatter.parse(ResourceTable.Layout_fraction_main, null, false);
        Component containerLayout = scatter.parse(ResourceTable.Layout_fraction_main, null, false);
        initComponent(containerLayout);
        EventBus.getDefault().register(this);

        addTabSelectedListener();
        return containerLayout;
    }

    private void initComponent(Component containerLayout) {
        tabType = (TabList) containerLayout.findComponentById(ResourceTable.Id_tab_list);
        tabSense = (StackLayout) containerLayout.findComponentById(ResourceTable.Id_fraction_main_sense);

        homeName = (Text) containerLayout.findComponentById(ResourceTable.Id_fraction_main_text_number);

        deviceSv = (ScrollView) containerLayout.findComponentById(ResourceTable.Id_fraction_main_deviceScrollView);
        deviceSv.setVisibility(Component.HIDE);

        deviceListLayout = (DirectionalLayout) containerLayout.findComponentById(ResourceTable.Id_fraction_main_deviceList);
        containerLayout.findComponentById(ResourceTable.Id_fraction_main_img_add).setClickedListener(component -> deviceAddDialog());
        initTab();
        containerLayout.findComponentById(ResourceTable.Id_fraction_main_sense).setClickedListener(component -> {
            EventBus.getDefault().post(new ChangeTabEvent(2));
            FractionUtil.replaceFraction(getFractionAbility(), ResourceTable.Id_main_dl_container, new ScheduleFraction());
        });
    }

    @Override
    public void onActive() {
        super.onActive();
        KLog.i(TAG, "===onActive");
        homeName.setText(PreferencesUtil.getUserName(getContext()));
        if (tabType.getSelectedTabIndex() == 1) {
            KLog.i(TAG, "===updateDeviceList");
            updateDeviceList();
        }
    }

    @Override
    protected void onStop() {
        EventBus.getDefault().unregister(this);
        super.onStop();
    }

    private void updateDeviceList() {
        getDeviceList();
    }

    private void initDeviceType() {
        if (typeIdAndCategory != null && !typeIdAndCategory.isEmpty()) {
            return;
        }
        List<DevicesType> devicesTypeList = PreferencesUtil.getDeviceType(getContext());
        typeIdAndCategory = new HashMap<>();
        devicesTypeList.forEach(item -> typeIdAndCategory.put(item.getId(), item.getCategory()));
    }

    private void initTab() {
        if (tabType.getTabCount() == 0) {
            tabType.addTab(createTab("场景"));
            tabType.addTab(createTab("设备"));
            tabType.getTabAt(0).select();
        }
    }

    //点击添加图标，弹出添加设备对话框
    private void deviceAddDialog() {
        CommonDialog commonDialog = new CommonDialog(getContext());
        commonDialog.setSize(Util.vp2px(getContext(), 104), Util.vp2px(getContext(), 132));
        commonDialog.setAlignment(LayoutAlignment.TOP);
        commonDialog.setOffset(Util.vp2px(getContext(), 114), Util.vp2px(getContext(), 60));
        commonDialog.setCornerRadius(Util.vp2px(getContext(), 4));
        Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_custom_dialog,
                null, false);
        commonDialog.setContentCustomComponent(container);
        commonDialog.setAutoClosable(true);
        commonDialog.show();
        addDeviceListener(container, commonDialog);
    }

    private void createNetConfigDialog() {
        CommonDialog commonDialog = new CommonDialog(getContext());
        commonDialog.setSize(Util.vp2px(getContext(), 336), Util.vp2px(getContext(), 328));
        commonDialog.setAlignment(LayoutAlignment.BOTTOM);
        commonDialog.setOffset(0, Util.vp2px(getContext(), 12));
        commonDialog.setCornerRadius(Util.vp2px(getContext(), 24));
        Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_dialog_netconfig,
                null, false);
        commonDialog.setContentCustomComponent(container);
        commonDialog.setAutoClosable(true);
        commonDialog.show();
    }

    private void addDeviceListener(Component component, CommonDialog dialog) {
        Button addSense = (Button) component.findComponentById(ResourceTable.Id_addSense);
        addSense.setClickedListener(comp -> {
            XToastUtils.info("暂未开放，敬请期待！");
            dialog.destroy();
        });
        Button takePhoto = (Button) component.findComponentById(ResourceTable.Id_addDevice);
        Button scanQrcode = (Button) component.findComponentById(ResourceTable.Id_sweep);
        takePhoto.setClickedListener(comp -> {
            createNetConfigDialog();
            dialog.destroy();
        });

        scanQrcode.setClickedListener(comp -> {
            Intent intent = new Intent();
            Operation operation =
                    new Intent.OperationBuilder()
                            .withBundleName(getBundleName())
                            .withAbilityName(ScanAbility.class.getName())
                            .build();
            intent.setOperation(operation);
            intent.setParam("scantag", 2);
            startAbility(intent, 0);
            dialog.destroy();
        });
    }

    private void addTabSelectedListener() {
        tabType.addTabSelectedListener(new TabList.TabSelectedListener() {
            @Override
            public void onSelected(TabList.Tab tab) {
                int position = tab.getPosition();
                if (position == 1) {
                    tabSense.setVisibility(Component.HIDE);
                    deviceSv.setVisibility(Component.VISIBLE);
                    updateDeviceList();
                } else {
                    tabSense.setVisibility(Component.VISIBLE);
                    deviceSv.setVisibility(Component.HIDE);
                }
            }

            @Override
            public void onUnselected(TabList.Tab tab) {
            }

            @Override
            public void onReselected(TabList.Tab tab) {
            }
        });
    }

    private TabList.Tab createTab(String text) {
        TabList.Tab tab = tabType.new Tab(this);
        tab.setText(text);
        return tab;
    }

    private void delete(String deviceId, String userId) {
        OKHttpUtilsRx2.INSTANCE.getApi().deleteDevice(deviceId, userId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            XToastUtils.toast("删除设备成功");
                            updateDeviceList();
                        } else if ("E21003".equals(userInfo.getErrCode())) {
                            XToastUtils.warning("无权限删除分享给你的设备");
                        }
                    }
                });
    }

    private DirectionalLayout createListView(DeviceResult item) {
        // 跟随设备名称展示对应图片
        Image deviceImage = new Image(getContext());

        // 设备名称
        Text deviceName = new Text(getContext());
        deviceName.setText(item.getName());
        deviceName.setTextSize(Util.fp2px(getContext(), 14));
        deviceName.setTextColor(new Color(Color.getIntColor("#000000")));
        // 设备是否在线
        Button deviceState = new Button(getContext());
        deviceState.setTextSize(Util.fp2px(getContext(), 10));
        deviceState.setWidth(Util.vp2px(getContext(), 36));
        deviceState.setHeight(Util.vp2px(getContext(), 16));
        deviceState.setMarginLeft(Util.vp2px(getContext(), 129));
        deviceState.setTextAlignment(TextAlignment.CENTER);
        if (item.isOnline() || item.getTypeId() == 5 || item.getTypeId() == 6 || item.getTypeId() == 7) {
            deviceName.setTextColor(new Color(Color.getIntColor("#000000")));

            switch (item.getTypeId()) {
                case 5: {
                    deviceImage.setPixelMap(ResourceTable.Media_icon_fraction_phone_online);
                    break;
                }
                case 6: {
                    deviceImage.setPixelMap(ResourceTable.Media_icon_fraction_watch2_online);
                    break;
                }
                case 7: {
                    deviceImage.setPixelMap(ResourceTable.Media_icon_fraction_pad_online);
                    break;
                }
                default: {
                    try {
                        Map<String, DeviceDataBean> map = DeviceData.INSTANCE.getDeviceData();
                        String productId = item.getId().split("_")[0];
                        deviceImage.setPixelMap(map.get(productId).getOnlineImage());
                    } catch (Exception e) {
                        XToastUtils.warning("设备在线状态图片加载错误");
                    }
                }
            }

            deviceState.setTextColor(new Color(Color.getIntColor("#0A59F7")));
            ShapeElement elementOnline = new ShapeElement();
            elementOnline.setShape(ShapeElement.RECTANGLE);
            float[] radii = {0, 0, Util.vp2px(getContext(), 8), Util.vp2px(getContext(), 8), 0, 0, Util.vp2px(getContext(), 8), Util.vp2px(getContext(), 8)};
            elementOnline.setCornerRadiiArray(radii);
            elementOnline.setRgbColor(new RgbColor(38, 110, 251, 25));
            deviceState.setBackground(elementOnline);
            deviceState.setText("在线");
        } else {
            deviceName.setTextColor(new Color(Color.argb(40, 0, 0, 0)));
            switch (item.getTypeId()) {
                case 5: {
                    deviceImage.setPixelMap(ResourceTable.Media_icon_fraction_phone_offline);
                    break;
                }
                case 6: {
                    deviceImage.setPixelMap(ResourceTable.Media_icon_fraction_watch2_offline);
                    break;
                }
                case 7: {
                    deviceImage.setPixelMap(ResourceTable.Media_icon_fraction_pad_offline);
                    break;
                }
                default: {
                    try {
                        Map<String, DeviceDataBean> map = DeviceData.INSTANCE.getDeviceData();
                        String productId = item.getId().split("_")[0];
                        deviceImage.setPixelMap(map.get(productId).getOfflineImage());
                    } catch (Exception e) {
                        XToastUtils.warning("设备离线状态图片加载错误");
                    }
                }
            }

            deviceState.setTextColor(new Color(Color.argb(40, 0, 0, 0)));
            ShapeElement elementOffline = new ShapeElement();
            elementOffline.setShape(ShapeElement.RECTANGLE);
            float[] radii = {0, 0, Util.vp2px(getContext(), 8), Util.vp2px(getContext(), 8), 0, 0, Util.vp2px(getContext(), 8), Util.vp2px(getContext(), 8)};
            elementOffline.setCornerRadiiArray(radii);
            elementOffline.setRgbColor(new RgbColor(0, 0, 0, 13));
            deviceState.setBackground(elementOffline);
            deviceState.setText("离线");
        }

        // 来自谁的设备
        Text deviceOwner = new Text(getContext());
        if (item.getOwnerName().equals(PreferencesUtil.getUserName(getContext()))) {
            deviceOwner.setText("来自我的设备");
        } else {
            deviceOwner.setText("来自" + item.getOwnerName() + "的设备");
        }
        deviceOwner.setTextSize(Util.fp2px(getContext(), 12));
        deviceOwner.setMultipleLine(true);
        deviceOwner.setTextColor(new Color(Color.argb(40, 0, 0, 0)));

        // 将设备名称和来自谁的设备加入到小垂直布局
        ComponentContainer directionalLayoutVertical = new DirectionalLayout(getContext());
        directionalLayoutVertical.setWidth(Util.vp2px(getContext(), 107));
        directionalLayoutVertical.addComponent(deviceName);
        directionalLayoutVertical.addComponent(deviceOwner);

        // 新建水平布局
        DirectionalLayout directionalLayoutHorizontal = new DirectionalLayout(getContext());
        directionalLayoutHorizontal.setOrientation(Component.HORIZONTAL);
        directionalLayoutHorizontal.setAlignment(LayoutAlignment.VERTICAL_CENTER);

        // 将小垂直布局和设备图标加入水平布局
        directionalLayoutHorizontal.addComponent(deviceImage);
        directionalLayoutHorizontal.addComponent(directionalLayoutVertical);

        // 将水平布局和设备是否在线加入完整布局
        DirectionalLayout allLayout = new DirectionalLayout(getContext());
        allLayout.setWidth(Util.vp2px(getContext(), 165));
        allLayout.setHeight(Util.vp2px(getContext(), 88));
        allLayout.setMarginLeft(Util.vp2px(getContext(), 10));
        allLayout.setMarginBottom(Util.vp2px(getContext(), 5));
        ShapeElement element = new ShapeElement();
        element.setShape(ShapeElement.RECTANGLE);
        element.setCornerRadius(Util.vp2px(getContext(), 8));
        element.setRgbColor(new RgbColor(Color.getIntColor("#FFFFFF")));
        allLayout.setBackground(element);

        allLayout.addComponent(deviceState);
        allLayout.addComponent(directionalLayoutHorizontal);

        return allLayout;
    }

    private void getDeviceList() {
        initDeviceType();
        deviceListLayout.removeAllComponents();
        String userId = PreferencesUtil.getUserId(getContext());
        if (StringUtils.isEmpty(userId)) {
            return;
        }
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceListByUserId(userId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<DeviceResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<DeviceResult>> deviceList) {
                        super.onNext(deviceList);

                        portableDevices.clear();
                        homeDevices.clear();
                        entertainDevices.clear();
                        monitoringDevices.clear();

                        deviceList.getResult().forEach(item -> {
                            // 通过Category 0是随身设备，1是家务设备，2是娱乐设备，3是监控设备
                            int category;
                            try {
                                category = typeIdAndCategory.get(item.getTypeId());
                            } catch (Exception e) {
                                XToastUtils.warning("category数值出错");
                                KLog.e("category数值出错");
                                KLog.e(typeIdAndCategory);
                                KLog.e(item.getTypeId());
                                category = 1;
                            }

                            if (0 == category) {
                                // 出现第一个随身设备时，显示随身设备
                                if (portableDevices.isEmpty()) {
                                    // 只需要设置一次随身设备
                                    portableEquipment = new Text(getContext());
                                    portableEquipment.setText("随身设备");
                                    portableEquipment.setTextSize(Util.fp2px(getContext(), 14));
                                    portableEquipment.setMarginLeft(Util.vp2px(getContext(), 16));
                                    portableEquipment.setTextColor(new Color(Color.argb(90, 0, 0, 0)));
                                    portableEquipment.setMarginBottom(Util.vp2px(getContext(), 10));

                                    // 新建随身设备列表
                                    portableEquipmentLayout = new TableLayout(getContext());
                                    portableEquipmentLayout.setColumnCount(2);
                                }
                                portableDevices.add(item);
                                DirectionalLayout portableLayout = createListView(item);
                                portableEquipmentLayout.addComponent(portableLayout);
                                onClickLayout(portableEquipmentLayout, portableDevices);
                                longClickLayout(portableEquipmentLayout, portableDevices);
                            }
                            if (1 == category) {
                                // 出现第一个家务设备时，显示家务设备
                                if (homeDevices.isEmpty()) {
                                    // 只需要设置一次家务设备
                                    homeEquipment = new Text(getContext());
                                    homeEquipment.setText("家务设备");
                                    homeEquipment.setTextSize(Util.fp2px(getContext(), 14));
                                    homeEquipment.setMarginLeft(Util.vp2px(getContext(), 16));
                                    homeEquipment.setTextColor(new Color(Color.argb(90, 0, 0, 0)));
                                    homeEquipment.setMarginBottom(Util.vp2px(getContext(), 10));

                                    // 新建家务设备列表
                                    homeEquipmentLayout = new TableLayout(getContext());
                                    homeEquipmentLayout.setColumnCount(2);
                                }
                                homeDevices.add(item);
                                DirectionalLayout homeLayout = createListView(item);
                                homeEquipmentLayout.addComponent(homeLayout);
                                onClickLayout(homeEquipmentLayout, homeDevices);
                                longClickLayout(homeEquipmentLayout, homeDevices);
                            }
                            if (2 == category) {
                                // 出现第一个娱乐设备时，显示娱乐设备
                                if (entertainDevices.isEmpty()) {
                                    // 只需要设置一次娱乐设备
                                    entertainEquipment = new Text(getContext());
                                    entertainEquipment.setText("娱乐设备");
                                    entertainEquipment.setTextSize(Util.fp2px(getContext(), 14));
                                    entertainEquipment.setMarginLeft(Util.vp2px(getContext(), 16));
                                    entertainEquipment.setTextColor(new Color(Color.argb(90, 0, 0, 0)));
                                    entertainEquipment.setMarginBottom(Util.vp2px(getContext(), 10));

                                    // 新建娱乐设备列表
                                    entertainEquipmentLayout = new TableLayout(getContext());
                                    entertainEquipmentLayout.setColumnCount(2);
                                }
                                entertainDevices.add(item);
                                DirectionalLayout entertainLayout = createListView(item);
                                entertainEquipmentLayout.addComponent(entertainLayout);
                                onClickLayout(entertainEquipmentLayout, entertainDevices);
                                longClickLayout(entertainEquipmentLayout, entertainDevices);
                            }
                            if (3 == category) {
                                // 出现第一个监控设备时，显示监控设备
                                if (monitoringDevices.isEmpty()) {
                                    // 只需要设置一次娱乐设备
                                    monitoringEquipment = new Text(getContext());
                                    monitoringEquipment.setText("监控设备");
                                    monitoringEquipment.setTextSize(Util.fp2px(getContext(), 14));
                                    monitoringEquipment.setMarginLeft(Util.vp2px(getContext(), 16));
                                    monitoringEquipment.setTextColor(new Color(Color.argb(90, 0, 0, 0)));
                                    monitoringEquipment.setMarginBottom(Util.vp2px(getContext(), 10));

                                    // 新建娱乐设备列表
                                    monitoringEquipmentLayout = new TableLayout(getContext());
                                    monitoringEquipmentLayout.setColumnCount(2);
                                }
                                monitoringDevices.add(item);

                                DirectionalLayout monitoringLayout = createListView(item);
                                monitoringEquipmentLayout.addComponent(monitoringLayout);
                                onClickLayout(monitoringEquipmentLayout, monitoringDevices);
                                longClickLayout(monitoringEquipmentLayout, monitoringDevices);
                            }
                        });
                        if (!portableDevices.isEmpty()) {
                            deviceListLayout.addComponent(portableEquipment);
                            deviceListLayout.addComponent(portableEquipmentLayout);
                        }
                        if (!homeDevices.isEmpty()) {
                            deviceListLayout.addComponent(homeEquipment);
                            deviceListLayout.addComponent(homeEquipmentLayout);
                        }
                        if (!entertainDevices.isEmpty()) {
                            deviceListLayout.addComponent(entertainEquipment);
                            deviceListLayout.addComponent(entertainEquipmentLayout);
                        }
                        if (!monitoringDevices.isEmpty()) {
                            deviceListLayout.addComponent(monitoringEquipment);
                            deviceListLayout.addComponent(monitoringEquipmentLayout);
                        }
                    }
                });
    }

    private void onClickLayout(TableLayout tableLayout, List<DeviceResult> devices) {
        int childNum = tableLayout.getChildCount();
        for (int index = 0; index < childNum; index++) {
            int currentIndex = index;
            Component child = tableLayout.getComponentAt(index);
            child.setClickedListener(component -> {
                String deviceId = devices.get(currentIndex).getId();
                int typeId = devices.get(currentIndex).getTypeId();
                if (5 == typeId || 6 == typeId || 7 == typeId) {
                    XToastUtils.toast("随身设备暂无控制页面");
                } else if (devices.get(currentIndex).isOnline()) {
                    // typeId == 1 为台灯，typeId ==2 为扫地机器人，typeId == 3 为护花使者 ，typeId == 4 为窗帘，5为手机，6为手表， 7 为pad
                    CommonUtil.judgeDevice(getContext(), deviceId, new Intent());
                } else {
                    XToastUtils.toast("该设备不在线，不能进入控制页面");
                }
            });
        }
    }

    //点击删除设备列表某一设备，弹出添加设备对话框
    private void deleteDeviceDialog(int index, List<DeviceResult> deleteDevices) {
        CommonDialog deleteDeviceDialog = new CommonDialog(getContext());
        deleteDeviceDialog.setSize(Util.vp2px(getContext(), 336), Util.vp2px(getContext(), 114));
        deleteDeviceDialog.setAlignment(LayoutAlignment.BOTTOM);
        deleteDeviceDialog.setOffset(0, Util.vp2px(getContext(), 15));
        deleteDeviceDialog.setCornerRadius(Util.vp2px(getContext(), 24));
        Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_delete_device_dialog,
                null, false);
        deleteDeviceDialog.setContentCustomComponent(container);
        deleteDeviceDialog.setAutoClosable(true);
        deleteDeviceDialog.show();

        Text deviceName = (Text) container.findComponentById(ResourceTable.Id_delete_device_name);
        Button deleteCancel = (Button) container.findComponentById(ResourceTable.Id_delete_device_cancel);
        Button deleteOK = (Button) container.findComponentById(ResourceTable.Id_delete_device_ok);

        deviceName.setText(String.format(Locale.CHINA, "确定删除%s设备", deleteDevices.get(index).getName()));
        deleteCancel.setClickedListener(component1 -> deleteDeviceDialog.destroy());
        deleteOK.setClickedListener(component1 -> {
            delete(deleteDevices.get(index).getId(), PreferencesUtil.getUserId(getContext()));
            deleteDeviceDialog.destroy();
        });
    }

    // 长按设备弹出对话框
    private void longClickDialog(int index, int[] position, List<DeviceResult> device) {
        CommonDialog longClickDialog = new CommonDialog(getContext());
        longClickDialog.setSize(Util.vp2px(getContext(), 104), Util.vp2px(getContext(), 132));
        longClickDialog.setAlignment(LayoutAlignment.TOP);
        longClickDialog.setOffset(position[0], position[1]);
        longClickDialog.setCornerRadius(Util.vp2px(getContext(), 4));
        Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_long_click_dialog,
                null, false);
        longClickDialog.setContentCustomComponent(container);
        longClickDialog.setAutoClosable(true);
        longClickDialog.show();

        Button editName = (Button) container.findComponentById(ResourceTable.Id_edit_device_name);
        Button shareDevice = (Button) container.findComponentById(ResourceTable.Id_share_device);
        Button deleteDevice = (Button) container.findComponentById(ResourceTable.Id_delete_device);
        editName.setClickedListener(component1 -> {

            Intent intent = new Intent();
            Operation operation = new Intent.OperationBuilder()
                    .withBundleName(getBundleName())
                    .withAbilityName(UpdateDeviceNameAbility.class.getName())
                    .build();
            intent.setOperation(operation);
            intent.setParam("deviceId", device.get(index).getId());
            intent.setParam("deviceName", device.get(index).getName());
            startAbility(intent, 0);
            longClickDialog.destroy();
        });
        shareDevice.setClickedListener(component1 -> {
            Intent intent = new Intent();
            Operation operation = new Intent.OperationBuilder()
                    .withBundleName(getBundleName())
                    .withAbilityName(ScheduleManageAbility.class.getName())
                    .withAction("action.shareDevice")
                    .build();
            intent.setParam("isShowMyself", false);
            intent.setOperation(operation);
            shareDeviceId = device.get(index).getId();
            intent.setParam("shareDeviceId", shareDeviceId);
            startAbility(intent, 0);
            longClickDialog.destroy();
        });
        deleteDevice.setClickedListener(component1 -> {
            deleteDeviceDialog(index, device);
            longClickDialog.destroy();
        });
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onShowMessageEvent(MessageEvent messageEvent) {
        KLog.i("=====event====", messageEvent);
        if (MessageEvent.EVENT_GET_SHARE_MEMBER.equals(messageEvent.getKey())) {
            String memberId = messageEvent.getMessage();
            if (!StringUtils.isBlank(memberId)) {
                String[] shareUserId = memberId.split(",");
                for (String id : shareUserId) {
                    shareDeviceToMember(id);
                }
            }
        } else {
            KLog.e("eventbus key错误");
        }
    }

    private void shareDeviceToMember(String shareUserIds) {
        String userId = PreferencesUtil.getUserId(getContext());
        if (StringUtils.isEmpty(userId)) {
            return;
        }
        OKHttpUtilsRx2.INSTANCE.getApi().shareDevice(userId, shareUserIds, shareDeviceId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        String message = userInfo.getMessage();
                        getContext().getUITaskDispatcher().asyncDispatch(() -> {
                            if (!StringUtils.isBlank(message)) {
                                if ("illegal permission".equals(message)) {
                                    XToastUtils.warning("只有设备拥有者才能分享");
                                }
                            } else if (!userInfo.isSuccess()) {
                                KLog.e(userInfo.getMessage());
                                XToastUtils.warning(userInfo.getMessage());
                            } else {
                                XToastUtils.toast("共享设备成功");
                            }
                        });
                    }
                });
    }

    private void longClickLayout(TableLayout tableLayout, List<DeviceResult> devices) {
        int childNum = tableLayout.getChildCount();
        for (int index = 0; index < childNum; index++) {
            int currentIndex = index;
            Component child = tableLayout.getComponentAt(index);
            child.setLongClickedListener(component -> {
                int[] position = component.getLocationOnScreen();
                longClickDialog(currentIndex, position, devices);
            });
        }
    }
}