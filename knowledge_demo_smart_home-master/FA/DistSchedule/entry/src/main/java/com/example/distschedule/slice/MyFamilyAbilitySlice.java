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
import com.example.distschedule.bean.FamilyMemberResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.event.FamilyNameEvent;
import com.example.distschedule.provider.MemberOfFamilyListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.*;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.components.element.StateElement;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.List;

public class MyFamilyAbilitySlice extends AbilitySlice implements Component.ClickedListener {
    private static final String TAG = MyFamilyAbilitySlice.class.getSimpleName();

    private MemberOfFamilyListProvider memberOfFamilyListProvider;
    private String familyId;
    private Text textName;
    private String familyName;
    private Text textFamilyName;
    private Switch mSwitchShowNickname;
    private String creatorId;
    private Text textCount;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_family);
        EventBus.getDefault().register(this);
        if (intent != null) {
            familyId = intent.getStringParam("familyId");
            familyName = intent.getStringParam("familyName");
            creatorId = intent.getStringParam("creatorId");
        }
        initComponents();
        initData();
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onShowMessageEvent(FamilyNameEvent familyNameEvent) {
        textFamilyName.setText(familyNameEvent.getFamilyName());
    }

    @Override
    public void onActive() {
        super.onActive();
        textName.setText(PreferencesUtil.getUserName(getContext()));
        initFamilyMembersData();
    }


    private void initData() {
        textFamilyName.setText(familyName);
    }

    private void initFamilyMembersData() {
        OKHttpUtilsRx2.INSTANCE.getApi().getFamilyMembersByFamilyId(familyId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<FamilyMemberResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<FamilyMemberResult>> memberList) {
                        super.onNext(memberList);
                        if (memberList.getResult() != null && memberList.getResult().size() > 0) {
                            textCount.setText("共" + memberList.getResult().size() + "人");
                            // 只有一个人的时候 值有添加按钮
                            if (memberList.getResult().size() == 1) {
                                for (int i = 0; i < 1; i++) {
                                    FamilyMemberResult familyMemberResult = new FamilyMemberResult();
                                    familyMemberResult.setTag(1);
                                    memberList.getResult().add(familyMemberResult);
                                }
                            } else {
                                // 人数超过一个人的时候 有添加也有删除按钮
                                for (int i = 0; i < memberList.getResult().size(); i++) {
                                    FamilyMemberResult familyMemberResult = memberList.getResult().get(i);
                                    if (PreferencesUtil.getUserId(getContext()).equals(familyMemberResult.getUserId())) {
                                    }
                                }

                                for (int i = 0; i < 2; i++) {
                                    FamilyMemberResult familyMemberResult = new FamilyMemberResult();
                                    if (i == 0) {
                                        familyMemberResult.setTag(1);
                                    } else {
                                        familyMemberResult.setTag(2);
                                    }
                                    memberList.getResult().add(familyMemberResult);
                                }
                            }
                            memberOfFamilyListProvider.setDate(memberList.getResult());
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        KLog.e(TAG, throwable.getMessage());
                        XToastUtils.toast("调用初始化家庭接口，返回失败");
                    }
                });
    }

    @Override
    protected void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }


    private void initComponents() {
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_family_dl_back);
        ListContainer listContainer = (ListContainer) findComponentById(ResourceTable.Id_family_ls_listContainer);
        DirectionalLayout familyName = (DirectionalLayout) findComponentById(ResourceTable.Id_family_dl_familyname);
        DirectionalLayout familyQRCode = (DirectionalLayout) findComponentById(ResourceTable.Id_family_dl_qrcode);
        DirectionalLayout myNickName = (DirectionalLayout) findComponentById(ResourceTable.Id_family_dl_myNickname);
        Button btExitFamily = (Button) findComponentById(ResourceTable.Id_family_bt_exits);
        textName = (Text) findComponentById(ResourceTable.Id_myfamilu_text_name);
        textFamilyName = (Text) findComponentById(ResourceTable.Id_myfamilu_text_familyname);
        mSwitchShowNickname = (Switch) findComponentById(ResourceTable.Id_myfamily_switch_showNicname);
        textCount = (Text) findComponentById(ResourceTable.Id_family_text_count);
        initSwitch();
        memberOfFamilyListProvider = new MemberOfFamilyListProvider(this);
        TableLayoutManager tableLayoutManager = new TableLayoutManager();
        tableLayoutManager.setColumnCount(5);
        listContainer.setLayoutManager(tableLayoutManager);
        listContainer.setItemProvider(memberOfFamilyListProvider);
        listContainer.setItemClickedListener((listContainer1, component, position, l) -> {
            if (memberOfFamilyListProvider.getData().get(position).getTag() == 1) {
                Intent intent = new Intent();
                intent.setParam("familyId", familyId);
                intent.setParam("creatorId", creatorId);
                present(new AddMemberAbilitySlice(), intent);
            } else if (memberOfFamilyListProvider.getData().get(position).getTag() == 2) {
                memberOfFamilyListProvider.setDeleteIcon();
            }
        });

        memberOfFamilyListProvider.setOnDeleteMemberListener(position -> {
            if (memberOfFamilyListProvider.getData() != null && memberOfFamilyListProvider.getData().get(position).getTag() == 0)
                deleteMember(position);
        });

        mSwitchShowNickname.setCheckedStateChangedListener((absButton, ischecked) -> memberOfFamilyListProvider.setShowNickname(ischecked));

        dlBack.setClickedListener(this);
        familyName.setClickedListener(this);
        familyQRCode.setClickedListener(this);
        myNickName.setClickedListener(this);
        btExitFamily.setClickedListener(this);
    }

    private void initSwitch() {
        ShapeElement elementThumbOn = new ShapeElement();
        elementThumbOn.setShape(ShapeElement.OVAL);
        elementThumbOn.setRgbColor(RgbColor.fromArgbInt(0xFFFFFFFF));
        elementThumbOn.setCornerRadius(50);
        // 关闭状态下滑块的样式
        ShapeElement elementThumbOff = new ShapeElement();
        elementThumbOff.setShape(ShapeElement.OVAL);
        elementThumbOff.setRgbColor(RgbColor.fromArgbInt(0xFFFFFFFF));
        elementThumbOff.setCornerRadius(50);
        // 开启状态下轨迹样式
        ShapeElement elementTrackOn = new ShapeElement();
        elementTrackOn.setShape(ShapeElement.RECTANGLE);
        elementTrackOn.setRgbColor(RgbColor.fromArgbInt(0xFF0A59F7));
        elementTrackOn.setCornerRadius(50);
        // 关闭状态下轨迹样式
        ShapeElement elementTrackOff = new ShapeElement();
        elementTrackOff.setShape(ShapeElement.RECTANGLE);
        elementTrackOff.setRgbColor(RgbColor.fromArgbInt(0xFF808080));
        elementTrackOff.setCornerRadius(50);

        mSwitchShowNickname.setTrackElement(trackElementInit(elementTrackOn, elementTrackOff));
        mSwitchShowNickname.setThumbElement(thumbElementInit(elementThumbOn, elementThumbOff));
    }

    private StateElement trackElementInit(ShapeElement on, ShapeElement off) {
        StateElement trackElement = new StateElement();
        trackElement.addState(new int[]{ComponentState.COMPONENT_STATE_CHECKED}, on);
        trackElement.addState(new int[]{ComponentState.COMPONENT_STATE_EMPTY}, off);
        return trackElement;
    }

    private StateElement thumbElementInit(ShapeElement on, ShapeElement off) {
        StateElement thumbElement = new StateElement();
        thumbElement.addState(new int[]{ComponentState.COMPONENT_STATE_CHECKED}, on);
        thumbElement.addState(new int[]{ComponentState.COMPONENT_STATE_EMPTY}, off);
        return thumbElement;
    }


    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_family_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_family_dl_qrcode:
                Intent intent = new Intent();
                intent.setParam("familyId", familyId);
                intent.setParam("creatorId", creatorId);
                present(new FamilyQrCodeAbilitySlice(), intent);
                break;
            case ResourceTable.Id_family_dl_myNickname:
                present(new UserNameAbilitySlice(), new Intent());
                break;
            case ResourceTable.Id_family_bt_exits:
                exitFamily();
                break;
            case ResourceTable.Id_family_dl_familyname:
                Intent intent_familyName = new Intent();
                intent_familyName.setParam("familyId", familyId);
                present(new FamilyNameAbilitySlice(), intent_familyName);
                break;
            default:
                break;
        }
    }

    // 删除家庭成员
    private void deleteMember(int position) {
        String memberID = memberOfFamilyListProvider.getData().get(position).getUserId();
        OKHttpUtilsRx2.INSTANCE.getApi().deleteFamilyMember(familyId, memberID, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            XToastUtils.toast("删除成功");
                            initFamilyMembersData();
                        }
                    }
                });
    }

    /**
     * 删除并退出家庭
     */
    private void exitFamily() {
        OKHttpUtilsRx2.INSTANCE.getApi().deleteFamily(familyId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            XToastUtils.toast("删除成功");
                            onBackPressed();
                        }
                    }
                });
    }
}
