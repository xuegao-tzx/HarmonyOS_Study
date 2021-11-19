/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "camera_ability_slice.h"
#include <algorithm>
#include <meta_data.h>
#include <window/window.h>
#include "ability_env.h"

#include "ability_manager.h"
#include "gfx_utils/color.h"
#include "securec.h"
#include "ui_camera_config.h"

namespace OHOS {
CameraAbilitySlice::~CameraAbilitySlice()
{
    if (cam_manager != nullptr) {
        delete cam_manager;
    }

    if (surfaceView != nullptr) {
        delete surfaceView;
    }
}

void CameraAbilitySlice::OnStart()
{
    surfaceView = new UISurfaceView();
    surfaceView->SetPosition(START_X, START_Y, SCREEN_WIDTH, SCREEN_HEIGHT - TITLE_HEIGHT);
    surfaceView->GetSurface()->SetWidthAndHeight(IMAGE_WIDTH, IMAGE_HEIGHT);

    cam_manager = new SampleCameraManager();
    cam_manager->SampleCameraCreate();

    int timecnt = 0;
    const int tryTimes = 6;
    while (1) {
        if (++timecnt > tryTimes) {
            printf("wait camera timeout!! \n");
            break;
        }

        if (cam_manager->SampleCameraIsReady()) {
            break;
        }

        sleep(1);
    }
    swipeView_->Add(surfaceView);
    swipeView_->Invalidate();
}

void CameraAbilitySlice::OnActive()
{
    if (cam_manager != nullptr && !cameraStatus_ && cam_manager->SampleCameraIsReady()) {
        cam_manager->SampleCameraStart(surfaceView->GetSurface());
        cameraStatus_ = true;
    }
}

void CameraAbilitySlice::OnBackground()
{
    if (cam_manager != nullptr && cameraStatus_) {
        cam_manager->SampleCameraStop();
        cameraStatus_ = false;
    }
}
}
