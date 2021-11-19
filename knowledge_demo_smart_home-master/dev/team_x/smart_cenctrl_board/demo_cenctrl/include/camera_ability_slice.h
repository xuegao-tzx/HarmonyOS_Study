/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_MAIN_ABILITY_SLICE_H
#define OHOS_MAIN_ABILITY_SLICE_H

#include <ability_loader.h>
#include <functional>
#include <utility>
#include <securec.h>
#include <stdio.h>
#include <components/ui_surface_view.h>
#include <components/ui_swipe_view.h>

#include "camera_manager.h"

namespace OHOS {
class CameraAbilitySlice : public AbilitySlice {
public:
    CameraAbilitySlice(UISwipeView *view) : swipeView_(view)
    {
        cameraStatus_ = false;
    }
    ~CameraAbilitySlice();

    void OnStart();
    void OnActive();
    void OnBackground();

private:
    SampleCameraManager *cam_manager {nullptr};
    UISurfaceView *surfaceView {nullptr};
    UISwipeView *swipeView_ {nullptr};
    bool cameraStatus_;
};
}

#endif // OHOS_MAIN_ABILITY_SLICE_H
