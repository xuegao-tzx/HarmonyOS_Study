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

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.FamilyAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.FamilyResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.Button;
import ohos.agp.components.DependentLayout;
import ohos.agp.components.TextField;
import ohos.app.Context;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.apache.commons.lang3.StringUtils;

public class CreateFamilyAbilitySlice extends AbilitySlice {
    private static final String TAG = CreateFamilyAbilitySlice.class.getSimpleName();
    private Button createFamilyBtn;
    private TextField familyNameTextFiled;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_family_create);
        initComponents();
    }

    private void initComponents() {
        DependentLayout backText = (DependentLayout) findComponentById(ResourceTable.Id_createFamily_dl_back);
        backText.setClickedListener(component -> onBackPressed());
        createFamilyBtn = (Button) findComponentById(ResourceTable.Id_button_create);
        createFamilyBtn.setClickedListener(component -> createFamily(familyNameTextFiled.getText(), PreferencesUtil.getUserId(this)));
        familyNameTextFiled = (TextField) findComponentById(ResourceTable.Id_textFiled_familyName);
        familyNameTextFiled.addTextObserver((String text, int start, int before, int count) ->
                createFamilyBtn.setEnabled(!StringUtils.isEmpty(text)));
    }

    private void createFamily(String familyName, String userId) {
        JSONObject body = new JSONObject();
        body.put("name", familyName);
        Context that = this;
        OKHttpUtilsRx2.INSTANCE.getApi().createFamily(RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)), userId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<FamilyResult>>() {
                    @Override
                    public void onNext(RespondBase<FamilyResult> familyResponse) {
                        super.onNext(familyResponse);
                        if (familyResponse.isSuccess()) {
                            XToastUtils.toast("创建成功");
                            PreferencesUtil.putUserFamilyInfo(that, familyResponse.getResult().getId(), familyName);
                            Intent intent = new Intent();
                            Operation operation =
                                    new Intent.OperationBuilder()
                                            .withBundleName(getBundleName())
                                            .withAbilityName(FamilyAbility.class.getName())
                                            .withAction("action.createFamilySuccess")
                                            .build();
                            intent.setParam("familyId", familyResponse.getResult().getId());
                            intent.setParam("creatorId", familyResponse.getResult().getCreatorId());
                            intent.setParam("familyName", familyName);

                            intent.setOperation(operation);
                            startAbility(intent);
                            onBackPressed();
                        } else {
                            KLog.e(TAG, familyResponse.getMessage());
                            XToastUtils.toast("创建家庭失败");
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        KLog.e(TAG, throwable.getMessage());
                        XToastUtils.toast("调用创建家庭接口，返回失败");
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
}
