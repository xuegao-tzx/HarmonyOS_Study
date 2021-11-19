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
import com.example.distschedule.ScanAbility;
import com.example.distschedule.bean.FamilyMemberResult;
import com.example.distschedule.bean.FamilyResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.UserResult;
import com.example.distschedule.event.MessageEvent;
import com.example.distschedule.provider.ShareDeviceMemberListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.*;
import org.apache.commons.lang3.StringUtils;
import org.greenrobot.eventbus.EventBus;

import java.util.List;

public class ShareDeviceAbilitySlice extends AbilitySlice implements Component.ClickedListener {

    private DependentLayout familyParent;
    private DependentLayout personalParent;
    private ShareDeviceMemberListProvider provider;
    private Text familyName;
    private DependentLayout shareList;
    private String shareDeviceId;
    private Button btShare;
    private boolean isExtend = true;
    private ListContainer listContainer;
    private Image imgExtend;
    private boolean isAllSelected = false;
    private TextField tfPhone;

    @Override
    protected void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_sharedevice);
        if (intent != null) {
            shareDeviceId = intent.getStringParam("shareDeviceId");
        }
        initComponents();
    }

    @Override
    protected void onActive() {
        super.onActive();
        requestFamily();
    }

    private void requestFamily() {
        OKHttpUtilsRx2.INSTANCE.getApi().getFamiliesByUserId(PreferencesUtil.getUserId(this))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<FamilyResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<FamilyResult>> respond) {
                        super.onNext(respond);
                        List<FamilyResult> results = respond.getResult();
                        if (respond.isSuccess() && results != null && !results.isEmpty()) {
                            familyName.setText(results.get(0).getName());
                            requestMemberById(results.get(0).getId());
                        }
                    }
                });
    }

    private void requestMemberById(String familyId) {
        OKHttpUtilsRx2.INSTANCE.getApi().getFamilyMembersByFamilyId(familyId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<FamilyMemberResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<FamilyMemberResult>> respond) {
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            provider.setData(respond.getResult());
                        }
                    }
                });
    }

    private void initComponents() {
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_inputname_dl_back);
        TabList tablist = (TabList) findComponentById(ResourceTable.Id_shareDevice_tab_list);
        familyParent = (DependentLayout) findComponentById(ResourceTable.Id_shareDevice_dl_family_parent);
        personalParent = (DependentLayout) findComponentById(ResourceTable.Id_shareDevice_dl_personal_parent);
        listContainer = (ListContainer) findComponentById(ResourceTable.Id_shareDevice_listContainer_member);
        familyName = (Text) findComponentById(ResourceTable.Id_shareDevice_text_familyName);
        shareList = (DependentLayout) findComponentById(ResourceTable.Id_shareDevice_dl_list);
        btShare = (Button) findComponentById(ResourceTable.Id_shareDevice_bt_share);
        DirectionalLayout dlExtend = (DirectionalLayout) findComponentById(ResourceTable.Id_shareDevice_dl_kuozhan);
        imgExtend = (Image) findComponentById(ResourceTable.Id_shareDevice_img_extend);
        Text textAllSelected = (Text) findComponentById(ResourceTable.Id_shareDevice_text_allSelected);
        tfPhone = (TextField) findComponentById(ResourceTable.Id_shareDevice_tf_phone);
        Image imgScan = (Image) findComponentById(ResourceTable.Id_shareDevice_img_scan);
        Button btSure = (Button) findComponentById(ResourceTable.Id_shareDevice_bt_sure);

        TableLayoutManager tableLayoutManager = new TableLayoutManager();
        tableLayoutManager.setColumnCount(1);
        listContainer.setLayoutManager(tableLayoutManager);
        provider = new ShareDeviceMemberListProvider(this);
        listContainer.setItemProvider(provider);
        TabList.Tab tabFamily = tablist.new Tab(getContext());
        tabFamily.setText("家庭");
        tablist.addTab(tabFamily);

        TabList.Tab tabPersonal = tablist.new Tab(getContext());
        tabPersonal.setText("个人");
        tablist.addTab(tabPersonal);
        tabFamily.select();
        dlBack.setClickedListener(this);
        shareList.setClickedListener(this);
        btShare.setClickedListener(this);
        dlExtend.setClickedListener(this);
        textAllSelected.setClickedListener(this);
        imgScan.setClickedListener(this);
        btSure.setClickedListener(this);
        tablist.addTabSelectedListener(new TabList.TabSelectedListener() {
            @Override
            public void onSelected(TabList.Tab tab) {
                int position = tab.getPosition();
                if (position == 0) {
                    familyParent.setVisibility(Component.VISIBLE);
                    personalParent.setVisibility(Component.HIDE);
                } else {
                    familyParent.setVisibility(Component.HIDE);
                    personalParent.setVisibility(Component.VISIBLE);
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

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_inputname_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_shareDevice_dl_list:
                Intent intent = new Intent();
                intent.setParam("shareDeviceId", shareDeviceId);
                present(new ShareListAbilitySlice(), intent);
                break;
            case ResourceTable.Id_shareDevice_bt_share:
                shareDevice();
                break;
            case ResourceTable.Id_shareDevice_dl_kuozhan:
                isExtend();
                break;
            case ResourceTable.Id_shareDevice_text_allSelected:
                isAllSelected();
                break;
            case ResourceTable.Id_shareDevice_img_scan:
                toScan();
                break;
            case ResourceTable.Id_shareDevice_bt_sure:
                getInfo();
                break;
            default:
                break;
        }
    }

    private void toScan() {
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(getBundleName())
                        .withAbilityName(ScanAbility.class.getName())
                        .build();
        intent.setOperation(operation);
        intent.setParam("scantag", 3);
        intent.setParam("shareDeviceId", shareDeviceId);
        startAbility(intent, 0);
    }

    private void getInfo() {
        String strPhone = tfPhone.getText();
        if (StringUtils.isBlank(strPhone)) {
            XToastUtils.toast("请输入电话号码");
            return;
        }
        if (strPhone.length() != 11) {
            XToastUtils.toast("请输入正确号码");
        }
        OKHttpUtilsRx2.INSTANCE.getApi().getUserInfo(tfPhone.getText())
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<UserResult>>() {
                    @Override
                    public void onNext(RespondBase<UserResult> userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess() && userInfo.getResult() != null)
                            sendMemberIds(userInfo.getResult().getId().toString());
                        onBackPressed();
                    }
                });
    }


    private void isAllSelected() {
        isAllSelected = !isAllSelected;
        provider.setIsAllSelected(isAllSelected);
    }

    private void isExtend() {
        isExtend = !isExtend;
        if (isExtend) {
            listContainer.setVisibility(Component.VISIBLE);
            imgExtend.setPixelMap(ResourceTable.Media_icon_group_close);
        } else {
            listContainer.setVisibility(Component.HIDE);
            imgExtend.setPixelMap(ResourceTable.Media_icon_group_open);
        }
    }

    private void shareDevice() {
        String memberId = provider.getMemberId();
        if (StringUtils.isBlank(memberId)) {
            XToastUtils.toast("您还没有选择");
            return;
        }
        sendMemberIds(memberId);
        onBackPressed();
    }

    private void sendMemberIds(String memberId) {
        EventBus.getDefault().post(new MessageEvent(MessageEvent.EVENT_GET_SHARE_MEMBER, memberId));
    }

}
