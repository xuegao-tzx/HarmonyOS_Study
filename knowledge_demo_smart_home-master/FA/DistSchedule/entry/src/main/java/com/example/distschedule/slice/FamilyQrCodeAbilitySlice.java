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
import com.example.distschedule.utils.QRCode;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.DependentLayout;
import ohos.agp.components.Image;
import ohos.media.image.PixelMap;

public class FamilyQrCodeAbilitySlice extends AbilitySlice {

    public static QRCode qrCode;
    private Image imgQrCode;
    private String familyId;
    private String creatorId;

    @Override
    protected void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_familyqrcode);
        if (intent != null) {
            familyId = intent.getStringParam("familyId");
            creatorId = intent.getStringParam("creatorId");
        }
        initComponents();
        if (null == qrCode) {
            qrCode = new QRCode(this);
        }
        showQrCode();
    }

    private void showQrCode() {
        int size = (qrCode.getScreenResolution(getContext()).getPointXToInt()
                - (10 + 40) * 2) / 2;
        PixelMap pixelMap = MainAbilitySlice.qrCode.generateCommonQRCode(familyId + "," + creatorId, size);
        imgQrCode.setPixelMap(pixelMap);
    }

    private void initComponents() {
        imgQrCode = (Image) findComponentById(ResourceTable.Id_myQrCodeSlice_img_qrcode);
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_myQrcode_dl_back);
        dlBack.setClickedListener(component -> onBackPressed());
    }
}
