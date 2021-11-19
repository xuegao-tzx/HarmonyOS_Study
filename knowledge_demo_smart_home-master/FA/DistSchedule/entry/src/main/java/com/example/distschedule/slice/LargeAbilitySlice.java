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
import com.example.distschedule.utils.CameraUtil;
import com.example.distschedule.utils.CommonUtil;
import com.j256.ormlite.utils.LogUtil;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.ability.DataAbilityHelper;
import ohos.aafwk.ability.DataAbilityRemoteException;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Button;
import ohos.agp.components.Component;
import ohos.agp.components.Image;
import ohos.agp.window.service.DisplayAttributes;
import ohos.agp.window.service.DisplayManager;
import ohos.media.image.ImageSource;
import ohos.media.image.common.Size;
import ohos.media.photokit.metadata.AVStorage;
import ohos.utils.net.Uri;

import java.io.FileDescriptor;
import java.io.FileNotFoundException;

public class LargeAbilitySlice extends AbilitySlice {
    private static final String TAG = LargeAbilitySlice.class.getSimpleName();
    private Image image;

    // 获取屏幕的宽高度
    private int width;
    private int height;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_large);

        Button button = (Button) findComponentById(ResourceTable.Id_image_button);

        // 为按钮设置点击事件回调
        button.setClickedListener(component -> {
            // 此处添加点击按钮后的事件处理逻辑
            recognize();
        });

        Component imageComponent = findComponentById(ResourceTable.Id_image);
        if (imageComponent != null && imageComponent instanceof Image) {
            image = (Image) imageComponent;
        }

        // 获取屏幕的宽高度
        DisplayAttributes attributes = DisplayManager.getInstance()
                .getDefaultDisplay(getContext()).get().getAttributes();
        width = attributes.width;
        height = attributes.height;

        int imageId = intent.getIntParam("imageId", 0);
        if (imageId == 0) {
            initView(intent);
        } else {
            showImageById(intent);
        }
    }

    private void initView(Intent intent) {
        LogUtil.info(TAG, "initView");
        if (findComponentById(ResourceTable.Id_image) instanceof Image) {
            image = (Image) findComponentById(ResourceTable.Id_image);
        }
        if (intent.getStringParam(CameraUtil.CAMERA_IMAGE_FILE_PATH) != null
                && !intent.getStringParam(CameraUtil.CAMERA_IMAGE_FILE_PATH).isEmpty()) {
            setImage(intent.getStringParam(CameraUtil.CAMERA_IMAGE_FILE_PATH));
        }
    }

    private void setImage(String picPath) {
        if (picPath != null && !picPath.isEmpty()) {
            CameraUtil.pixelMap = CameraUtil.getPixelMap(null, picPath, 1);
            image.setPixelMap(CameraUtil.pixelMap);
        }
    }

    private void recognize() {
    }

    // 通过文件Id去加载本地图片
    private void showImageById(Intent intent) {
        int imageId = intent.getIntParam("imageId", 0);
        ImageSource imageSource = null;
        DataAbilityHelper helper = DataAbilityHelper.creator(this);
        try {
            Uri uri = Uri.appendEncodedPathToUri(AVStorage.Images.Media.EXTERNAL_DATA_ABILITY_URI,
                    String.valueOf(imageId));
            FileDescriptor fd = helper.openFile(uri, "r");
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            decodingOpts.desiredSize = new Size(width, height / CommonUtil.Num.FOUR * CommonUtil.Num.THREE);
            imageSource = ImageSource.create(fd, null);
            CameraUtil.pixelMap = imageSource.createPixelmap(decodingOpts);
            image.setPixelMap(CameraUtil.pixelMap);
        } catch (FileNotFoundException | DataAbilityRemoteException e) {
            LogUtil.error(TAG, "FileNotFoundException | DataAbilityRemoteException" + e);
        } finally {
            if (imageSource != null) {
                imageSource.release();
            }
        }
    }
}
