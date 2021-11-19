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

import com.example.distschedule.FamilyAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.FamilyResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.provider.FamilyListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.slice.MainAbilitySlice;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.*;
import ohos.agp.utils.LayoutAlignment;
import ohos.media.image.PixelMap;

import java.util.List;

/**
 * 我的页面
 *
 * @since 2021-08-28
 */
public class MyFraction extends FractionBase {
    private static final String TAG = MyFraction.class.getName();
    private DirectionalLayout containerLayout;
    private DirectionalLayout emptyFamilyLayout;
    private DirectionalLayout familyListLayout;
    private Text familyCountText;
    private ListContainer familiesContainer;
    private Component topAddContainer;
    private FamilyListProvider familyListProvider;
    private Text deviceCount;
    private Text userNameText;

    @Override
    protected Component onComponentAttached(LayoutScatter scatter, ComponentContainer container, Intent intent) {

        containerLayout = (DirectionalLayout) LayoutScatter.getInstance(getApplicationContext()).parse(ResourceTable.Layout_fraction_my, null, false);
        containerLayout.findComponentById(ResourceTable.Id_qr_code_parent).setClickedListener(this::personalInfo);

        // 个人资料
        topAddContainer = LayoutScatter.getInstance(getApplicationContext()).parse(ResourceTable.Layout_top_personal_info, null, false);
        topAddContainer.findComponentById(ResourceTable.Id_top_img_back).setClickedListener(this::back);

        initComponents(containerLayout);
        return containerLayout;
    }


    @Override
    protected void onActive() {
        super.onActive();
        initFamilyData();
        getDeviceList();
        userNameText.setText(PreferencesUtil.getUserName(this));
    }

    private void getDeviceList() {
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceListByUserId(PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<DeviceResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<DeviceResult>> deviceList) {
                        super.onNext(deviceList);
                        if (deviceList.getResult() == null || deviceList.getResult().size() == 0) {
                            deviceCount.setText("共绑定0台设备");
                        } else {
                            deviceCount.setText("共绑定" + deviceList.getResult().size() + "台设备");
                        }
                    }
                });
    }

    private void initFamilyData() {
        MyFraction that = this;
        OKHttpUtilsRx2.INSTANCE.getApi().getFamiliesByUserId(PreferencesUtil.getUserId(this))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<FamilyResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<FamilyResult>> families) {
                        super.onNext(families);
                        familyCountText.setText("(" + families.getResult().size() + ")");
                        if (families.getResult().size() == 0) {
                            that.emptyFamilyLayout.setVisibility(Component.VISIBLE);
                            that.familyListLayout.setVisibility(Component.HIDE);
                        } else {
                            that.emptyFamilyLayout.setVisibility(Component.HIDE);
                            that.familyListLayout.setVisibility(Component.VISIBLE);
                            familyListProvider = new FamilyListProvider(families.getResult(), that);
                            familiesContainer.setItemProvider(familyListProvider);
                            familyListProvider.notifyDataChanged();
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        KLog.e(TAG, throwable.getMessage());
                        XToastUtils.toast("初始化家庭数据失败");
                    }
                });
    }

    private void initComponents(Component containerLayout) {
        userNameText = (Text) containerLayout.findComponentById(ResourceTable.Id_text_user_name);
        deviceCount = (Text) containerLayout.findComponentById(ResourceTable.Id_text_device_count);
        Text createFamilyText = (Text) containerLayout.findComponentById(ResourceTable.Id_text_create);
        createFamilyText.setClickedListener(component ->
        {
            Intent createFamilyIntent = new Intent();
            Operation operation = new Intent.OperationBuilder()
                    .withBundleName(getBundleName())
                    .withAbilityName(FamilyAbility.class.getName())
                    .build();
            createFamilyIntent.setOperation(operation);
            startAbility(createFamilyIntent, 0);
        });
        this.emptyFamilyLayout = (DirectionalLayout) containerLayout.findComponentById(ResourceTable.Id_empty_family_directionallayout);
        this.familyListLayout = (DirectionalLayout) containerLayout.findComponentById(ResourceTable.Id_familylist_directionallayout);
        this.familyCountText = (Text) containerLayout.findComponentById(ResourceTable.Id_family_count_text);
        this.familiesContainer = (ListContainer) containerLayout.findComponentById(ResourceTable.Id_families_container);

        containerLayout.findComponentById(ResourceTable.Id_family_dl_accountbook).setClickedListener(component -> {
            XToastUtils.info("暂未开放,敬请期待");
        });
        containerLayout.findComponentById(ResourceTable.Id_family_dl_buscard).setClickedListener(component -> {
            XToastUtils.info("暂未开放,敬请期待");
        });
        containerLayout.findComponentById(ResourceTable.Id_family_dl_qrcode).setClickedListener(component -> {
            XToastUtils.info("暂未开放,敬请期待");
        });
        containerLayout.findComponentById(ResourceTable.Id_family_dl_mycar).setClickedListener(component -> {
            XToastUtils.info("暂未开放,敬请期待");
        });

        familiesContainer.setItemClickedListener((listContainer, component, position, l) -> {
            Intent intent = new Intent();
            Operation operation =
                    new Intent.OperationBuilder()
                            .withBundleName(getBundleName())
                            .withAbilityName(FamilyAbility.class.getName())
                            .withAction("action.myfamily")
                            .build();

            intent.setParam("familyName", familyListProvider.getFamilyList().get(position).getName());
            intent.setParam("familyId", familyListProvider.getFamilyList().get(position).getId());
            intent.setParam("creatorId", familyListProvider.getFamilyList().get(position).getCreatorId());
            intent.setOperation(operation);
            startAbility(intent, 0);
        });
    }

    private void personalInfo(Component component) {
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(getBundleName())
                        .withAbilityName(FamilyAbility.class.getName())
                        .withAction("action.personalinfor")
                        .build();
        intent.setOperation(operation);
        startAbility(intent, 0);
    }

    private void setTopContainer(String title) {
        containerLayout.removeAllComponents();
        containerLayout.addComponent(topAddContainer);
        Text titleText = (Text) topAddContainer.findComponentById(ResourceTable.Id_top_text_title);
        titleText.setText(title);
    }


    private void back(Component component) {

    }

    private void createQrCode(Component component) {
        int size = (MainAbilitySlice.qrCode.getScreenResolution(getContext()).getPointXToInt()
                - (10 + 40) * 2) / 2;
        PixelMap pixelMap = MainAbilitySlice.qrCode.generateCommonQRCode("13012345678", size);

        setTopContainer("我的二维码");
        setQrCode(pixelMap);
    }

    private void setQrCode(PixelMap pixelMap) {
        Component emptyContainer = LayoutScatter.getInstance(getApplicationContext()).parse(ResourceTable.Layout_qr_code, null, false);
        Image qrCodeImage = (Image) emptyContainer.findComponentById(ResourceTable.Id_qr_code_big);
        qrCodeImage.setPixelMap(pixelMap);
        containerLayout.addComponent(emptyContainer);
        containerLayout.setAlignment(LayoutAlignment.START);
    }

}