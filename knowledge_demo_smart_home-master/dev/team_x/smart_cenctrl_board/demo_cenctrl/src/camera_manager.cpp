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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "camera_manager.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;


static constexpr int PIC_WIDTH = 1920;
static constexpr int PIC_HEIGHT = 1080;


// TestFrameStateCallback
void TestFrameStateCallback::OnFrameFinished(Camera &camera, FrameConfig &fc, FrameResult &result)
{
    if (fc.GetFrameConfigType() == FRAME_CONFIG_CAPTURE) {
        cout << "Capture frame received." << endl;
        list<Surface *> surfaceList = fc.GetSurfaces();

        for (Surface *surface : surfaceList) {
            SurfaceBuffer *buffer = surface->AcquireBuffer();
            if (buffer != nullptr) {
                char *virtAddr = static_cast<char *>(buffer->GetVirAddr());
                if (virtAddr != nullptr) {
                }
                surface->ReleaseBuffer(buffer);
            } else {
                cout << "ERROR:surface buffer is NULL!" << endl;
            }
            delete surface;
        }
        delete &fc;
    }
}

// SampleCameraStateMng class
SampleCameraStateMng::~SampleCameraStateMng()
{
    if (fc_) {
        delete fc_;
        fc_ = nullptr;
    }
}

void SampleCameraStateMng::OnCreated(Camera &c)
{
    cout << "Sample recv OnCreate camera." << endl;
    auto config = CameraConfig::CreateCameraConfig();
    if (config == nullptr) {
        cout << "New object failed." << endl;
        return;
    }
    config->SetFrameStateCallback(&fsCb_, &eventHdlr_);
    c.Configure(*config);
    cam_ = &c;
}

void SampleCameraStateMng::StartPreview(Surface *surface)
{
    if (cam_ == nullptr) {
        cout << "Camera is not ready." << endl;
        return;
    }
    if (gPreviewSta_ == MEDIA_STATE_START) {
        cout << "Camera is already previewing." << endl;
        return;
    }
    if (surface == nullptr) {
        cout << "surface is NULL." << endl;
        return;
    }

    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_PREVIEW);
    fc->AddSurface(*surface);
    int32_t ret = cam_->TriggerLoopingCapture(*fc);
    if (ret != 0) {
        delete fc;
        cout << "camera start preview failed. ret=" << ret << endl;
        return;
    }

    gPreviewSta_ =  MEDIA_STATE_START;
    cout << "camera start preview succeed." << endl;
}

void SampleCameraStateMng::StopPreview()
{
    if (cam_ == nullptr) {
        cout << "Camera is not ready." << endl;
        return;
    }

    cam_->StopLoopingCapture();
    gPreviewSta_ = MEDIA_STATE_IDLE;
}

bool SampleCameraStateMng::CameraIsReady()
{
    return (cam_ == nullptr) ? false : true;
}

// SampleCameraManager class
SampleCameraManager::~SampleCameraManager()
{
    if (CamStateMng) {
        CamStateMng->StopPreview();
        delete CamStateMng;
        CamStateMng = NULL;
    }
}

int SampleCameraManager::SampleCameraCreate()
{
    cout << "camera start init!" << endl;
    camKit = CameraKit::GetInstance();
    if (camKit == nullptr) {
        cout << "Can not get CameraKit instance" << endl;
        return -1;
    }

    list<string> camList = camKit->GetCameraIds();
    for (auto &cam : camList) {
        cout << "camera name:" << cam << endl;
        const CameraAbility *ability = camKit->GetCameraAbility(cam);
        /* find camera which fits user's ability */
        list<CameraPicSize> sizeList = ability->GetSupportedSizes(0);
        for (auto &pic : sizeList) {
            if (pic.width == PIC_WIDTH && pic.height == PIC_HEIGHT) {
                camId = cam;
                break;
            }
        }
    }

    if (camId.empty()) {
        cout << "No available camera.(1080p wanted)" << endl;
        return -1;
    }

    CamStateMng = new SampleCameraStateMng(eventHdlr_);
    if (CamStateMng == NULL) {
        cout << "create SampleCameraStateMng failed!" << endl;
        return -1;
    }

    camKit->CreateCamera(camId, *CamStateMng, eventHdlr_);

    cout << "camera init ok!" << endl;

    return 0;
}

bool SampleCameraManager::SampleCameraExist(void)
{
    return camId.empty() ? false : true;
}

int SampleCameraManager::SampleCameraStart(Surface *surface)
{
    if (CamStateMng == NULL)
        return -1;
    CamStateMng->StartPreview(surface);

    return 0;
}

int SampleCameraManager::SampleCameraStop(void)
{
    if (CamStateMng == nullptr)
        return -1;

    CamStateMng->StopPreview();

    return 0;
}

bool SampleCameraManager::SampleCameraIsReady(void)
{
    return CamStateMng->CameraIsReady();
}
