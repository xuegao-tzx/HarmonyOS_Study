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
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.QRCode;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.*;
import ohos.bundle.IBundleManager;
import ohos.media.image.PixelMap;
import ohos.security.SystemPermission;
import org.apache.commons.lang3.StringUtils;

public class AddMemberAbilitySlice extends AbilitySlice implements Component.ClickedListener {
    /**
     * request Code.
     */
    private static final int REQUEST_CODE = 20210601;
    public static QRCode qrCode;
    private Image imgQrCode;
    private DependentLayout dlBack;
    private Button btSure;
    private TextField textFieldPhone;
    private String familyId;
    private Image amgScan;
    private String creatorId;

    @Override
    protected void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_addmember);
        if (intent != null) {
            if (intent != null) {
                familyId = intent.getStringParam("familyId");
                creatorId = intent.getStringParam("creatorId");
            }
        }
        initComponents();
        if (null == qrCode) {
            qrCode = new QRCode(this);
        }
        showQrCode();
    }

    private void showQrCode() {
        int size = (qrCode.getScreenResolution(getContext()).getPointXToInt()
                - (120) * 2) / 2;
        PixelMap pixelMap = MainAbilitySlice.qrCode.generateCommonQRCode(familyId + "," + creatorId, size);
        imgQrCode.setPixelMap(pixelMap);
    }

    private void initComponents() {
        imgQrCode = (Image) findComponentById(ResourceTable.Id_addmemberslice_img_qrcode);
        dlBack = (DependentLayout) findComponentById(ResourceTable.Id_addmember_dl_back);
        btSure = (Button) findComponentById(ResourceTable.Id_addmember_bt_sure);
        textFieldPhone = (TextField) findComponentById(ResourceTable.Id_addmember_tf_phone);
        amgScan = (Image) findComponentById(ResourceTable.Id_addmember_img_scan);

        btSure.setClickedListener(this);
        dlBack.setClickedListener(this);
        amgScan.setClickedListener(this);
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_addmember_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_addmember_bt_sure:
                addMember();
                break;
            case ResourceTable.Id_addmember_img_scan:

                Intent intent = new Intent();
                Operation operation =
                        new Intent.OperationBuilder()
                                .withBundleName(getBundleName())
                                .withAbilityName(ScanAbility.class.getName())
                                .build();
                intent.setOperation(operation);
                intent.setParam("scantag", 1);
                intent.setParam("familyId", familyId);
                intent.setParam("creatorId", creatorId);
                startAbility(intent, 0);
                onBackPressed();
                break;
            default:
                break;
        }
    }

    /**
     * 申请权限.
     */
    private void requestPermission() {
        // 必须手动权限
        if (verifySelfPermission(SystemPermission.CAMERA)
                != IBundleManager.PERMISSION_GRANTED) {
            // has no permission
            if (canRequestPermission(SystemPermission.CAMERA)) {
                // toast
                requestPermissionsFromUser(
                        new String[]{SystemPermission.CAMERA}, REQUEST_CODE);
            }
        }
    }

    private void addMember() {
        String text = textFieldPhone.getText();
        if (StringUtils.isBlank(text)) {
            return;
        }
        OKHttpUtilsRx2.INSTANCE.getApi().createFamilyMember(familyId, text, PreferencesUtil.getUserName(getContext()), PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            textFieldPhone.setText("");
                            XToastUtils.toast("添加成功");
                            onBackPressed();
                        }
                    }
                });
    }
}
