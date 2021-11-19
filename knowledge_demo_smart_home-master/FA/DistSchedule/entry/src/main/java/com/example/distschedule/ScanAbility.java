/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

package com.example.distschedule;

import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.UserResult;
import com.example.distschedule.qrcode.QRCodeView;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import ohos.aafwk.ability.Ability;
import ohos.aafwk.content.Intent;
import ohos.agp.components.ComponentContainer;
import ohos.agp.components.DirectionalLayout;
import ohos.bundle.IBundleManager;
import ohos.security.SystemPermission;
import org.apache.commons.lang3.StringUtils;

public class ScanAbility extends Ability implements QRCodeView.ScanResultListener {
    /**
     * request Code.
     */
    private static final int REQUEST_CODE = 20210601;
    /**
     * 显示二维码扫描视图.
     */
    private QRCodeView qrCodeView;
    /**
     * scantag = 1 邀请成员,扫描个人二维码 familyId和creatorId是知道的,扫描结果为个人phoneNumber
     * scantag = 2 显示家庭二维码,扫描结果即为familyId+creatorId,中间用逗号隔开,个人电话号码是知道的
     * scantag = 3 扫描个人信息,获取电话号码  从而分享设备
     */
    private int scantag;
    private String familyId;
    private String creatorId;
    private String shareDeviceId;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        if (intent != null) {
            scantag = intent.getIntParam("scantag", 0);
            familyId = intent.getStringParam("familyId");
            creatorId = intent.getStringParam("creatorId");
            shareDeviceId = intent.getStringParam("shareDeviceId");
            if (scantag == 1 || scantag == 3) {
                XToastUtils.toast("请扫描个人二维码");
            } else if (scantag == 2) {
                XToastUtils.toast("请扫描家庭二维码");
            }
        }
        addComponent();
    }

    /**
     * 添加扫码布局
     */
    private void addComponent() {
        qrCodeView = new QRCodeView(this);
        DirectionalLayout.LayoutConfig params =
                new DirectionalLayout.LayoutConfig(
                        ComponentContainer.LayoutConfig.MATCH_PARENT,
                        ComponentContainer.LayoutConfig.MATCH_PARENT);
        qrCodeView.setLayoutConfig(params);
        qrCodeView.setScanResultListener(this);

        setUIContent(qrCodeView);
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


    @Override
    public void scanResult(String result) {
//        QRCodeUtil.showToast(getContext(), result);
        if (!StringUtils.isBlank(result)) {
            qrCodeView.onDestroy();
            if (scantag == 3) {
                shareDevice(result);
            } else {
                addMember(result);
            }

        }

    }

    private void shareDevice(String phone) {
        if (StringUtils.isBlank(phone) || phone.length() != 11) {
            XToastUtils.toast("号码有误" + phone);
            onBackPressed();
            return;
        }

        OKHttpUtilsRx2.INSTANCE.getApi().getUserInfo(phone)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<UserResult>>() {
                    @Override
                    public void onNext(RespondBase<UserResult> userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess() && userInfo.getResult() != null)
                            shareDeviceToMember(userInfo.getResult().getId().toString());
                    }
                });
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
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            XToastUtils.toast("共享设备成功");
                            onBackPressed();
                        }
                    }
                });
    }

    private void addMember(String result) {
        String phone = null;
        String strfamilyId = null;
        String userId = null;
        if (scantag == 1) {
            phone = result;
            strfamilyId = familyId;
            userId = creatorId;
        } else if (scantag == 2) {
            String[] split = result.split(",");
            if (split.length != 2) {
                return;
            }
            phone = PreferencesUtil.getUserPhone(getContext());

            strfamilyId = split[0];
            userId = split[1];
        }
        if (!StringUtils.isBlank(phone) && StringUtils.isBlank(strfamilyId) && !StringUtils.isBlank(userId)) {
            return;
        }
        OKHttpUtilsRx2.INSTANCE.getApi().createFamilyMember(strfamilyId, phone, PreferencesUtil.getUserName(getContext()), userId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase userInfo) {
                        super.onNext(userInfo);
                        if (userInfo.isSuccess()) {
                            XToastUtils.toast("添加成功");
                            onBackPressed();
                        }
                    }
                });
    }
}
