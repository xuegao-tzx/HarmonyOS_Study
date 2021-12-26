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
/*
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.ChildrenWatchContacts;
import com.example.distschedule.bean.Command;
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.CommandUtil;
import com.example.distschedule.utils.DeviceStateUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.Util;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;
import ohos.agp.utils.Color;
import ohos.agp.utils.LayoutAlignment;
import ohos.agp.window.dialog.CommonDialog;
import ohos.app.dispatcher.TaskDispatcher;
import ohos.app.dispatcher.task.TaskPriority;
import ohos.miscservices.timeutility.Time;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.apache.commons.lang3.StringUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import static com.example.distschedule.slice.SelectDeviceAbilitySlice.KEY_IS_SET_COMMAND;

**
 * 智能儿童手表
 *
 * @since 2021-08-28

public class ChildrenWatchAbilitySlice extends AbilitySlice {
    private final List<ChildrenWatchContacts> contacts = new ArrayList<>();
    private String longitude; // 经度
    private String latitude; // 纬度
    private String deviceId;
    private boolean isSetCommand = false;
    private Image deleteContactsImg;
    private Image addContactsImg;
    private Text contactsNumber;
    private int numbers = 0; // 联系人数量 最少为0 ，最大为2
    private Text positionText;

    // 截取小数点后两位
    private static String getRateStr(String rateStr) {

        int i = rateStr.indexOf(".");
        if (i != -1) {
            //获取小数点的位置
            int num = 0;
            num = rateStr.indexOf(".");
            System.out.println(num + "=============");

            //获取小数点后面的数字 是否有两位 不足两位补足两位
            String dianAfter = rateStr.substring(0, num + 1);
            String afterData = rateStr.replace(dianAfter, "");
            if (afterData.length() < 2) {
                afterData = afterData + "0";
            }
            return rateStr.substring(0, num) + "." + afterData.substring(0, 2);
        } else {
            rateStr = rateStr + ".00";
            return rateStr;
        }
    }

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_children_watch);
        deviceId = intent.getStringParam("deviceId");
        isSetCommand = intent.getBooleanParam(KEY_IS_SET_COMMAND, false);
        initComponents();
        initDeviceState();
    }

    private void initChildrenWatchState() {
        TaskDispatcher globalTaskDispatcher = getGlobalTaskDispatcher(TaskPriority.DEFAULT);
        globalTaskDispatcher.asyncDispatch(() -> {
            KLog.i("async task initLampState run");
            getDeviceInfo();
        });
    }

    private void initDeviceState() {
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        KLog.i(deviceInfo);
                        if (deviceInfo.isSuccess() && deviceInfo.getResult().isOnline()) {
                            // 更新状态
                            String property = deviceInfo.getResult().getProperty();
                            longitude = DeviceStateUtil.getKeyValue(property, "logitude");
                            latitude = DeviceStateUtil.getKeyValue(property, "latitude");
                            if (!longitude.isEmpty() && !latitude.isEmpty()) {
                                positionText.setText(getRateStr(longitude) + "," + getRateStr(latitude));
                            }
                            if (!StringUtils.isEmpty(DeviceStateUtil.getKeyValue(property, "father"))) {
                                numbers = numbers + 1;
                                contacts.add(new ChildrenWatchContacts(ResourceTable.Media_icon_unselected, "father", DeviceStateUtil.getKeyValue(property, "father"), false));
                            }
                            if (!StringUtils.isEmpty(DeviceStateUtil.getKeyValue(property, "mother"))) {
                                numbers = numbers + 1;
                                contacts.add(new ChildrenWatchContacts(ResourceTable.Media_icon_unselected, "mother", DeviceStateUtil.getKeyValue(property, "father"), false));
                            }
                            contactsNumber.setText(String.format(Locale.CHINA, "已添加%s个联系人", numbers));
                            addContactsImg.setPixelMap(ResourceTable.Media_icon_add_contacts);
                            deleteContactsImg.setPixelMap(ResourceTable.Media_icon_delet_contacts);
                            if (0 == numbers) {
                                contactsNumber.setText("暂无联系人");
                                deleteContactsImg.setPixelMap(ResourceTable.Media_icon_delete_contacts_unable);
                            } else if (2 == numbers) {
                                addContactsImg.setPixelMap(ResourceTable.Media_icon_add_contacts_unable);
                            }

                        } else {
                            initChildrenWatchState();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        // 失败是因为首次配网状态未更新
                        initChildrenWatchState();
                    }
                });
    }

    private void getDeviceInfo() {
        Time.sleep(10 * 1000);
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        super.onNext(deviceInfo);
                        KLog.i(deviceInfo);
                        if (deviceInfo.getResult() == null || !deviceInfo.getResult().isOnline()) {
                            XToastUtils.toast("未配网成功，请再次碰一碰重试");
                        }
                    }
                });
    }

    private void initComponents() {
        findComponentById(ResourceTable.Id_watch_img_left).setClickedListener(this::goBack);
        positionText = (Text) findComponentById(ResourceTable.Id_watch_text_postion);
        contactsNumber = (Text) findComponentById(ResourceTable.Id_watch_text_contacts_number);
        deleteContactsImg = (Image) findComponentById(ResourceTable.Id_watch_image_contacts_delete);
        deleteContactsImg.setClickedListener(component -> deleteListener());
        addContactsImg = (Image) findComponentById(ResourceTable.Id_watch_image_contacts_add);
        addContactsImg.setClickedListener(component -> addListener());

        // 顶部右侧图标
        if (isSetCommand) {
            Image rightIcon = (Image) findComponentById(ResourceTable.Id_watch_img_right);
            rightIcon.setPixelMap(ResourceTable.Media_icon_ok);
            rightIcon.setClickedListener(component -> saveCommand());
        }
    }

    private void goBack(Component component) {
        onBackPressed();
    }

    private void saveCommand() {
        onBackPressed();
    }

    private void deleteListener() {
        if (0 == numbers) {
            XToastUtils.info("暂无联系人，不能删除");
        } else {
            CommonDialog commonDialog = new CommonDialog(getContext());
            commonDialog.setAlignment(LayoutAlignment.BOTTOM);
            commonDialog.setSize(Util.vp2px(getContext(), 336), Util.vp2px(getContext(), 500));
            commonDialog.setCornerRadius(Util.vp2px(getContext(), 24));
            Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_delete_contacts_dialog,
                    null, false);
            commonDialog.setContentCustomComponent(container);
            commonDialog.setAutoClosable(true);
            DirectionalLayout deleteContactsLayout = (DirectionalLayout) container.findComponentById(ResourceTable.Id_delete_contacts_List);
            if (deleteContactsLayout.getChildCount() > 0) {
                deleteContactsLayout.removeAllComponents();
            }
            // 将水平布局加入table表
            TableLayout deleteListLayout = new TableLayout(getContext());
            for (ChildrenWatchContacts contact : contacts) {
                DirectionalLayout layout = createListView(contact);
                deleteListLayout.addComponent(layout);
                onClickSelect(deleteListLayout, contacts);
            }
            deleteContactsLayout.addComponent(deleteListLayout);
            commonDialog.show();
            deleteContactsListener(container, commonDialog);
        }
    }

    private DirectionalLayout createListView(ChildrenWatchContacts contact) {
        // 删除联系人名称
        Text deleteName = new Text(getContext());
        deleteName.setText(contact.getContactsName() + "(" + contact.getContactsNumber() + ")");
        deleteName.setTextSize(Util.fp2px(getContext(), 16));
        deleteName.setMarginLeft(Util.vp2px(getContext(), 20));
        deleteName.setTextColor(new Color(Color.getIntColor("#000000")));
        deleteName.setAlpha((float) 0.9);
        deleteName.setWidth(Util.vp2px(getContext(), 160));

        // 选取删除联系人图片
        Image deleteSelectImage = new Image(getContext());
        deleteSelectImage.setMarginLeft(Util.vp2px(getContext(), 100));
        deleteSelectImage.setPixelMap(ResourceTable.Media_icon_unselected);

        // 新建水平布局
        DirectionalLayout directionalLayoutHorizontal = new DirectionalLayout(getContext());
        directionalLayoutHorizontal.setOrientation(Component.HORIZONTAL);
        directionalLayoutHorizontal.setAlignment(LayoutAlignment.VERTICAL_CENTER);
        directionalLayoutHorizontal.setMarginTop(Util.fp2px(getContext(), 32));

        // 将联系人和删除图标加入水平布局
        directionalLayoutHorizontal.addComponent(deleteName);
        directionalLayoutHorizontal.addComponent(deleteSelectImage);

        return directionalLayoutHorizontal;
    }

    private void deleteContactsListener(Component component, CommonDialog dialog) {
        Image cancelImg = (Image) component.findComponentById(ResourceTable.Id_delete_contacts_dialog_cancel);
        Image okImg = (Image) component.findComponentById(ResourceTable.Id_delete_contacts_dialog_ok);

        cancelImg.setClickedListener(component1 -> dialog.destroy());
        okImg.setClickedListener(component12 -> {
            int j;
            for (j = 0; j < contacts.size(); j++) {
                if (contacts.get(j).getIsSelect()) {
                    break;
                }
            }
            if (j >= contacts.size()) {
                XToastUtils.info("请选择要删除的联系人，或取消删除");
            } else {
                deleteContacts(dialog);
            }
        });
    }

    private void onClickSelect(TableLayout tableLayout, List<ChildrenWatchContacts> list) {
        int childNum = tableLayout.getChildCount();
        for (int index = 0; index < childNum; index++) {
            int currentIndex = index;
            DirectionalLayout childLayout = (DirectionalLayout) (tableLayout.getComponentAt(currentIndex));
            childLayout.setClickedListener(component -> {
                Image images = (Image) (childLayout.getComponentAt(1));
                ChildrenWatchContacts device = list.get(currentIndex);
                if (device.getIsSelect()) {
                    device.setIsSelect(false);
                    images.setPixelMap(ResourceTable.Media_icon_unselected);
                } else {
                    device.setIsSelect(true);
                    images.setPixelMap(ResourceTable.Media_icon_selected);
                }
            });
        }

    }

    //点击删除设备列表某一设备
    private void deleteContacts(CommonDialog dialog) {
        CommonDialog deleteDialog = new CommonDialog(getContext());
        deleteDialog.setSize(Util.vp2px(getContext(), 336), Util.vp2px(getContext(), 144));
        deleteDialog.setAlignment(LayoutAlignment.BOTTOM);
        deleteDialog.setOffset(0, Util.vp2px(getContext(), 15));
        deleteDialog.setCornerRadius(Util.vp2px(getContext(), 24));
        Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_delete_dialog,
                null, false);
        deleteDialog.setContentCustomComponent(container);
        deleteDialog.setAutoClosable(true);
        deleteDialog.show();

        Text Name = (Text) container.findComponentById(ResourceTable.Id_delete_name);
        Button deleteCancel = (Button) container.findComponentById(ResourceTable.Id_delete_cancel);
        Button deleteOK = (Button) container.findComponentById(ResourceTable.Id_delete_ok);
        if (contacts.size() >= 2) {
            if (contacts.get(0).getIsSelect()) {
                Name.setText(String.format(Locale.CHINA, "确定删除紧急联系:“%s”吗?", contacts.get(0).getContactsName()));
            }
            if (contacts.get(1).getIsSelect()) {
                Name.setText(String.format(Locale.CHINA, "确定删除紧急联系:“%s”吗?", contacts.get(1).getContactsName()));
            }
            if (contacts.get(0).getIsSelect() && contacts.get(1).getIsSelect()) {
                Name.setText("确定删除所有紧急联系人吗?");
            }
        } else if (contacts.get(0).getIsSelect()) {
            Name.setText(String.format(Locale.CHINA, "确定删除紧急联系:“%s”吗?", contacts.get(0).getContactsName()));
        }
        deleteCancel.setClickedListener(component1 -> deleteDialog.destroy());
        deleteOK.setClickedListener(component1 -> {
            for (int i = contacts.size() - 1; i >= 0; i--) {
                if (contacts.get(i).getIsSelect()) {
                    sendCommand(CommandUtil.deleteWatchPhoneNumber(contacts.get(i).getContactsName(), contacts.get(i).getContactsNumber()));
                    contacts.remove(i);
                    numbers = numbers - 1;
                    if (numbers == 0) {
                        contactsNumber.setText("暂无联系人");
                        deleteContactsImg.setPixelMap(ResourceTable.Media_icon_delete_contacts_unable);
                    } else {
                        contactsNumber.setText(String.format(Locale.CHINA, "已添加%s个联系人", numbers));
                    }
                }
            }
            addContactsImg.setPixelMap(ResourceTable.Media_icon_add_contacts);
            deleteDialog.destroy();
            dialog.destroy();
        });
    }


    private void addListener() {
        if (2 == numbers) {
            XToastUtils.info("已经有2个联系人，不能再添加");
        } else {
            CommonDialog commonDialog = new CommonDialog(getContext());
            commonDialog.setAlignment(LayoutAlignment.BOTTOM);
            commonDialog.setSize(Util.vp2px(getContext(), 336), Util.vp2px(getContext(), 500));
            commonDialog.setCornerRadius(Util.vp2px(getContext(), 24));
            Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_add_contacts_dialog,
                    null, false);
            commonDialog.setContentCustomComponent(container);
            commonDialog.setAutoClosable(true);
            commonDialog.show();
            addContactsListener(container, commonDialog);
        }
    }

    private void addContactsListener(Component component, CommonDialog Dialog) {
        Image cancelImg = (Image) component.findComponentById(ResourceTable.Id_add_contacts_dialog_cancel);
        Image okImg = (Image) component.findComponentById(ResourceTable.Id_add_contacts_dialog_ok);
        Text contactsName = (Text) component.findComponentById(ResourceTable.Id_add_contacts_name);
        Text contactsPhoneNumber = (Text) component.findComponentById(ResourceTable.Id_add_contacts_number);

        cancelImg.setClickedListener(component1 -> Dialog.destroy());
        okImg.setClickedListener(component12 -> {
            numbers = numbers + 1;
            contactsNumber.setText(String.format(Locale.CHINA, "已添加%s个联系人", numbers));
            sendCommand(CommandUtil.addWatchPhoneNumber(contactsName.getText(), contactsPhoneNumber.getText()));
            deleteContactsImg.setPixelMap(ResourceTable.Media_icon_delet_contacts);
            contacts.add(new ChildrenWatchContacts(ResourceTable.Media_icon_unselected, contactsName.getText(), contactsPhoneNumber.getText(), false));
            Dialog.destroy();
            if (numbers == 2) {
                addContactsImg.setPixelMap(ResourceTable.Media_icon_add_contacts_unable);
            }
        });
    }

    private void sendCommand(Command command) {
        JSONObject body = new JSONObject();
        body.put("commandName", command.getCommandName());
        body.put("serviceId", command.getServiceId());
        body.put("value", command.getValue());
        OKHttpUtilsRx2.INSTANCE.getApi().sendCommand(deviceId, RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respond) {
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            XToastUtils.toast("发送命令成功");
                        }
                    }
                });
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }
}*/
