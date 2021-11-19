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

import com.example.distschedule.LargeAbility;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.utils.CameraUtil;
import com.example.distschedule.utils.MyDrawTask;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.Component;
import ohos.agp.components.ComponentContainer;
import ohos.agp.components.DirectionalLayout;
import ohos.agp.components.Image;
import ohos.agp.components.surfaceprovider.SurfaceProvider;
import ohos.agp.graphics.Surface;
import ohos.agp.graphics.SurfaceOps;
import ohos.app.Environment;
import ohos.eventhandler.EventHandler;
import ohos.eventhandler.EventRunner;
import ohos.eventhandler.InnerEvent;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.media.camera.CameraKit;
import ohos.media.camera.device.*;
import ohos.media.image.ImageReceiver;
import ohos.media.image.common.ImageFormat;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import static com.example.distschedule.utils.CameraUtil.CAMERA_IMAGE_FILE_PATH;
import static ohos.media.camera.device.Camera.FrameConfigType.FRAME_CONFIG_PICTURE;
import static ohos.media.camera.device.Camera.FrameConfigType.FRAME_CONFIG_PREVIEW;

public class TakePhotoAbilitySlice extends AbilitySlice {
    /**
     * DELAY_TIME
     *
     * @since 2021-01-19
     */
    public static final int DELAY_TIME = 200;
    private static final HiLogLabel TAG = new HiLogLabel(3, 0xD001100, "TakePhotoAbilitySlice");
    private static final int EVENT_IMAGESAVING_PROMTING = 0x0000024;
    private static final int SCREEN_WIDTH = 1080;
    private static final int SCREEN_HEIGHT = 2340;
    private static final int IMAGE_RCV_CAPACITY = 9;
    private static final String IMG_FILE_PREFIX = "IMG_";
    private static final String IMG_FILE_TYPE = ".png";
    private static final int SLEEP_TIME = 200;
    private Surface previewSurface;

    private SurfaceProvider surfaceProvider;

    private boolean isCameraRear;

    private Camera cameraDevice;

    private EventHandler creamEventHandler;
    private byte[] bytes;

    private ImageReceiver imageReceiver;

    private Image takePictureImage;

    private Image switchCameraImage;

    private File targetFile;

    private Image smallImage;
    /**
     * EventHandler
     *
     * @since 2021-08-19
     */
    private final EventHandler handler = new EventHandler(EventRunner.current()) {
        @Override
        protected void processEvent(InnerEvent event) {
            if (event.eventId == EVENT_IMAGESAVING_PROMTING) {
                HiLog.info(TAG, "EVENT_IMAGESAVING_PROMTING");
                MyDrawTask drawTask = new MyDrawTask(smallImage);
                smallImage.addDrawTask(drawTask);
                drawTask.putPixelMap(CameraUtil.getPixelMap(bytes, "", 1));
                smallImage.setEnabled(true);
            }
        }
    };

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_take_photo);
        initControlComponents();
        initSurface();
        creamEventHandler = new EventHandler(EventRunner.create("CameraBackground"));
    }

    private void initSurface() {
        surfaceProvider = new SurfaceProvider(this);
        DirectionalLayout.LayoutConfig params =
                new DirectionalLayout.LayoutConfig(
                        ComponentContainer.LayoutConfig.MATCH_PARENT, ComponentContainer.LayoutConfig.MATCH_PARENT);

        surfaceProvider.setLayoutConfig(params);
        surfaceProvider.pinToZTop(true);

        surfaceProvider.getSurfaceOps().get().addCallback(new SurfaceCallBack());
        Component surfaceContainer = findComponentById(ResourceTable.Id_surface_container);
        if (surfaceContainer instanceof ComponentContainer) {
            ((ComponentContainer) surfaceContainer).addComponent(surfaceProvider);
        }
    }

    private void initControlComponents() {
        if (findComponentById(ResourceTable.Id_tack_picture_btn) instanceof Image) {
            takePictureImage = (Image) findComponentById(ResourceTable.Id_tack_picture_btn);
        }
        takePictureImage.setClickedListener(this::takePicture);

        if (findComponentById(ResourceTable.Id_switch_camera_btn) instanceof Image) {
            switchCameraImage = (Image) findComponentById(ResourceTable.Id_switch_camera_btn);
            MyDrawTask drawTask = new MyDrawTask(switchCameraImage);
            switchCameraImage.addDrawTask(drawTask);
        }
        switchCameraImage.setClickedListener(component -> switchClicked());

        if (findComponentById(ResourceTable.Id_small_pic) instanceof Image) {
            smallImage = (Image) findComponentById(ResourceTable.Id_small_pic);
        }
        smallImage.setClickedListener(component -> showBigPic());
    }

    private void takePicture(Component component) {
        HiLog.info(TAG, "takePicture");
        if (!takePictureImage.isEnabled()) {
            HiLog.info(TAG, "takePicture return");
            return;
        }
        if (cameraDevice == null || imageReceiver == null) {
            return;
        }
        FrameConfig.Builder framePictureConfigBuilder = cameraDevice.getFrameConfigBuilder(FRAME_CONFIG_PICTURE);
        framePictureConfigBuilder.addSurface(imageReceiver.getRecevingSurface());
        FrameConfig pictureFrameConfig = framePictureConfigBuilder.build();
        cameraDevice.triggerSingleCapture(pictureFrameConfig);
    }

    private void switchClicked() {
        if (!takePictureImage.isEnabled()) {
            return;
        }
        takePictureImage.setEnabled(false);
        isCameraRear = !isCameraRear;
        CameraUtil.setIsCameraRear(isCameraRear);
        openCamera();
    }

    private void openCamera() {
        imageReceiver = ImageReceiver.create(SCREEN_WIDTH, SCREEN_HEIGHT, ImageFormat.JPEG, IMAGE_RCV_CAPACITY);
        imageReceiver.setImageArrivalListener(this::saveImage);
        CameraKit cameraKit = CameraKit.getInstance(getApplicationContext());
        String[] cameraList = cameraKit.getCameraIds();
        String cameraId = "";
        for (String logicalCameraId : cameraList) {
            int faceType = cameraKit.getCameraInfo(logicalCameraId).getFacingType();
            switch (faceType) {
                case CameraInfo.FacingType.CAMERA_FACING_FRONT:
                    if (isCameraRear) {
                        cameraId = logicalCameraId;
                    }
                    break;
                case CameraInfo.FacingType.CAMERA_FACING_BACK:
                    if (!isCameraRear) {
                        cameraId = logicalCameraId;
                    }
                    break;
                case CameraInfo.FacingType.CAMERA_FACING_OTHERS:
                default:
                    break;
            }
        }
        if (cameraId != null && !cameraId.isEmpty()) {
            CameraStateCallbackImpl cameraStateCallback = new CameraStateCallbackImpl();
            cameraKit.createCamera(cameraId, cameraStateCallback, creamEventHandler);
        }
    }

    private void saveImage(ImageReceiver receiver) {
        String fileName = IMG_FILE_PREFIX + System.currentTimeMillis() + IMG_FILE_TYPE;
        targetFile = new File(getExternalFilesDir(Environment.DIRECTORY_PICTURES), fileName);
        try {
            HiLog.info(TAG, "filePath is " + targetFile.getCanonicalPath());
        } catch (IOException e) {
            HiLog.error(TAG, "filePath is error");
        }

        ohos.media.image.Image image = receiver.readNextImage();
        if (image == null) {
            return;
        }
        ohos.media.image.Image.Component component = image.getComponent(ImageFormat.ComponentType.JPEG);
        bytes = new byte[component.remaining()];
        component.read(bytes);
        try (FileOutputStream output = new FileOutputStream(targetFile)) {
            output.write(bytes);
            output.flush();
            handler.sendEvent(EVENT_IMAGESAVING_PROMTING);
        } catch (IOException e) {
            HiLog.info(TAG, "IOException, Save image failed");
        }
    }

    private void releaseCamera() {
        if (cameraDevice != null) {
            cameraDevice.release();
            cameraDevice = null;
        }

        if (imageReceiver != null) {
            imageReceiver.release();
            imageReceiver = null;
        }

        if (creamEventHandler != null) {
            creamEventHandler.removeAllEvent();
            creamEventHandler = null;
        }
    }

    /**
     * showBigPic
     */
    private void showBigPic() {
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(getBundleName())
                        .withAbilityName(LargeAbility.class.getName())
                        .build();
        intent.setOperation(operation);
        if (targetFile != null) {
            try {
                intent.setParam(CAMERA_IMAGE_FILE_PATH, targetFile.getCanonicalPath());
            } catch (IOException e) {
                HiLog.error(TAG, "filePath is error");
            }
        }
        startAbility(intent);
    }

    @Override
    protected void onStop() {
        super.onStop();
        releaseCamera();
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    /**
     * CameraStateCallbackImpl
     *
     * @since 2021-03-08
     */
    class CameraStateCallbackImpl extends CameraStateCallback {
        CameraStateCallbackImpl() {
        }

        @Override
        public void onCreated(Camera camera) {
            previewSurface = surfaceProvider.getSurfaceOps().get().getSurface();
            if (previewSurface == null) {
                HiLog.info(TAG, "create camera filed, preview surface is null");
                return;
            }

            try {
                Thread.sleep(SLEEP_TIME);
            } catch (InterruptedException exception) {
                HiLog.info(TAG, "Waiting to be interrupted");
            }

            CameraConfig.Builder cameraConfigBuilder = camera.getCameraConfigBuilder();
            cameraConfigBuilder.addSurface(previewSurface);
            cameraConfigBuilder.addSurface(imageReceiver.getRecevingSurface());
            camera.configure(cameraConfigBuilder.build());
            cameraDevice = camera;

            enableImageGroup();
        }

        @Override
        public void onConfigured(Camera camera) {
            FrameConfig.Builder framePreviewConfigBuilder = camera.getFrameConfigBuilder(FRAME_CONFIG_PREVIEW);
            framePreviewConfigBuilder.addSurface(previewSurface);
            try {
                // 启动循环帧捕获
                camera.triggerLoopingCapture(framePreviewConfigBuilder.build());
            } catch (IllegalArgumentException e) {
                HiLog.error(TAG, "Argument Exception");
            } catch (IllegalStateException e) {
                HiLog.error(TAG, "State Exception");
            }
        }

        private void enableImageGroup() {
            takePictureImage.setEnabled(true);
            switchCameraImage.setEnabled(true);
        }
    }

    /**
     * SurfaceCallBack
     *
     * @since 2021-03-08
     */
    class SurfaceCallBack implements SurfaceOps.Callback {
        @Override
        public void surfaceCreated(SurfaceOps callbackSurfaceOps) {
            if (callbackSurfaceOps != null) {
                callbackSurfaceOps.setFixedSize(surfaceProvider.getHeight(), surfaceProvider.getWidth());
            }
            creamEventHandler.postTask(TakePhotoAbilitySlice.this::openCamera, DELAY_TIME);
        }

        @Override
        public void surfaceChanged(SurfaceOps callbackSurfaceOps, int format, int width, int height) {
        }

        @Override
        public void surfaceDestroyed(SurfaceOps callbackSurfaceOps) {
        }
    }
}
