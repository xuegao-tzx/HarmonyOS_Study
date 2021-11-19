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
import com.example.distschedule.bean.FamilyMemberResult;
import com.example.distschedule.bean.FamilyResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.event.MessageEvent;
import com.example.distschedule.provider.SelectMemberListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.Util;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.Component;
import ohos.agp.components.ListContainer;
import ohos.agp.components.Text;
import ohos.agp.components.element.ShapeElement;
import org.greenrobot.eventbus.EventBus;

import java.util.List;

public class SelectMemberAbilitySlice extends AbilitySlice {
    private SelectMemberListProvider provider;
    private String familyName = "";
    private String memberIds = "";
    private boolean isShowMyself = true;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_select_member);
        isShowMyself = intent.getBooleanParam("isShowMyself", true);
        memberIds = intent.getStringParam("memberIds");
        initComponents();
    }

    private void initComponents() {
        ListContainer listContainer = (ListContainer) findComponentById(ResourceTable.Id_selectMember_lc_list);
        ShapeElement background = new ShapeElement();
        background.setRgbColor(new RgbColor(255, 255, 255));
        background.setCornerRadius(Util.vp2px(this, 16));
        listContainer.setBackground(background);
        provider = new SelectMemberListProvider(this);
        listContainer.setItemProvider(provider);
        findComponentById(ResourceTable.Id_selectMember_img_confirm).setClickedListener(this::saveMember);
        findComponentById(ResourceTable.Id_selectMember_img_back).setClickedListener(component -> onBackPressed());
        findComponentById(ResourceTable.Id_selectMember_text_all).setClickedListener(component -> provider.selectAll());
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
                            familyName = results.get(0).getName();
                            ((Text) findComponentById(ResourceTable.Id_selectMember_text_familyName)).setText(familyName);
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
                            provider.setData(respond.getResult(), isShowMyself, memberIds);
                        }
                    }
                });
    }

    private void saveMember(Component component) {
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("familyName", familyName);
        jsonObject.put("memberIds", provider.getMemberId());
        EventBus.getDefault().post(new MessageEvent(MessageEvent.EVENT_GET_MEMBER, jsonObject));
        onBackPressed();
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
