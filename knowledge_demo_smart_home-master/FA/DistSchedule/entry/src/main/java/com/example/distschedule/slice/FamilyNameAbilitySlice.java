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
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.event.FamilyNameEvent;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.apache.commons.lang3.StringUtils;
import org.greenrobot.eventbus.EventBus;

public class FamilyNameAbilitySlice extends AbilitySlice implements Component.ClickedListener {
    private static final String KEY_USER_NAME = "User_Name";
    private TextField textFieldName;
    private String familyName;
    private String familyId;

    @Override
    protected void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_familyname);
        if (intent != null) {
            familyId = intent.getStringParam("familyId");

        }
        initComponents();
    }

    private void initComponents() {
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_inputname_dl_back);
        Button btSure = (Button) findComponentById(ResourceTable.Id_inputname_bt_sure);
        textFieldName = (TextField) findComponentById(ResourceTable.Id_inputname_textfield_name);
        Image imgDelete = (Image) findComponentById(ResourceTable.Id_inputname_img_delete);
        dlBack.setClickedListener(this);
        btSure.setClickedListener(this);
        imgDelete.setClickedListener(this);
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_inputname_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_inputname_bt_sure:
                updateUserName();
                break;
            case ResourceTable.Id_inputname_img_delete:
                textFieldName.setText("");
                break;
            default:
                break;
        }
    }

    private void updateUserName() {
        if (StringUtils.isEmpty(textFieldName.getText())) {
            return;
        }
        familyName = textFieldName.getText().trim();
        String userId = PreferencesUtil.getUserId(getContext());
        JSONObject body = new JSONObject();
        body.put("name", familyName);

        OKHttpUtilsRx2.INSTANCE.getApi().updateFamily(familyId, userId, RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respondBase) {
                        super.onNext(respondBase);
                        KLog.i(respondBase);
                        if (respondBase.isSuccess()) {
                            XToastUtils.toast("修改成功");
                            EventBus.getDefault().post(new FamilyNameEvent(familyName));
                            onBackPressed();
                        }
                    }

                });

    }
}
