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
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.UserShareResult;
import com.example.distschedule.provider.ShareListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;
import org.apache.commons.lang3.StringUtils;

import java.util.List;

public class ShareListAbilitySlice extends AbilitySlice implements Component.ClickedListener {

    private ShareListProvider provider;
    private String shareDeviceId;
    private DirectionalLayout showDataContainer;
    private DirectionalLayout emptyContainer;
    // 是否全选 默认是全选
    private boolean isAllSelected = true;
    private Button familySure;

    @Override
    protected void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_share_list);
        if (intent != null) {
            shareDeviceId = intent.getStringParam("shareDeviceId");
        }
        initComponents();
    }

    @Override
    protected void onActive() {
        super.onActive();
        getShareList();
    }

    private void getShareList() {
        OKHttpUtilsRx2.INSTANCE.getApi().getUserShare(shareDeviceId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<UserShareResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<UserShareResult>> respond) {
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            if (respond.getResult() == null || respond.getResult().size() == 0) {
                                showEmpty();
                            } else {
                                showDataView();
                            }
                            provider.setData(respond.getResult());
                        }
                    }
                });
    }

    private void showDataView() {
        showDataContainer.setVisibility(Component.VISIBLE);
        emptyContainer.setVisibility(Component.HIDE);
    }

    private void showEmpty() {
        showDataContainer.setVisibility(Component.HIDE);
        emptyContainer.setVisibility(Component.VISIBLE);
    }


    private void initComponents() {
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_inputname_dl_back);
        ListContainer listContainer = (ListContainer) findComponentById(ResourceTable.Id_shareList_listContainer);
        showDataContainer = (DirectionalLayout) findComponentById(ResourceTable.Id_sharelit_dl_show);
        emptyContainer = (DirectionalLayout) findComponentById(ResourceTable.Id_sharelit_dl_empty);
        DependentLayout dlAllSelected = (DependentLayout) findComponentById(ResourceTable.Id_shareDevice_dl_allSelected);
        familySure = (Button) findComponentById(ResourceTable.Id_shareList_bt_sure);


        TableLayoutManager tableLayoutManager = new TableLayoutManager();
        tableLayoutManager.setColumnCount(1);
        listContainer.setLayoutManager(tableLayoutManager);
        provider = new ShareListProvider(this);
        listContainer.setItemProvider(provider);
        dlBack.setClickedListener(this);
        dlAllSelected.setClickedListener(this);
        familySure.setClickedListener(this);
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_inputname_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_shareDevice_dl_allSelected:
                isAllSelected = !isAllSelected;
                provider.setIsAllSelected(isAllSelected);
                break;
            case ResourceTable.Id_shareList_bt_sure:
                unShareDevice();
                break;
            default:
                break;
        }
    }

    private void unShareDevice() {
        String memberId = provider.getMemberId();
        if (StringUtils.isBlank(memberId)) {
            XToastUtils.toast("您还没有选择");
            return;
        }

        String[] shareUserId = memberId.split(",");
        for (String shareId : shareUserId) {
            shareDeviceToMember(shareId);
        }
    }

    private void shareDeviceToMember(String shareUserIds) {
        String userId = PreferencesUtil.getUserId(getContext());
        if (StringUtils.isEmpty(userId)) {
            return;
        }
        OKHttpUtilsRx2.INSTANCE.getApi().unShareDevice(userId, shareUserIds, shareDeviceId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            XToastUtils.toast("操作成功");
                            onBackPressed();
                        }
                    }
                });
    }

}
