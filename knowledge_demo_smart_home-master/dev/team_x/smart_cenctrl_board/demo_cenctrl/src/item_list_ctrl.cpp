
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

#include <securec.h>
#include "item_list_ctrl.h"
#include "cenctrl_log.h"

namespace OHOS {
ItemCtrl::ItemCtrl()
{
    currState_ = false;
}

ItemCtrl::~ItemCtrl()
{
    currState_ = false;
}

void ItemCtrl::Setup(UIViewGroup* view, UILabel* label, UIImageView* image, std::string strUnSelect,
                     std::string strSelect, std::string devName)
{
    view_ = view;
    label_ = label;
    image_ = image;
    strSelect_ = strSelect;
    strUnSelect_ = strUnSelect;
    devName_ = devName;
}

bool ItemCtrl::OnClick(UIView& view, const ClickEvent& event)
{
    LOG(CENCTRL_DEBUG, "ItemClicked! \n");
    if (strSelect_ == "" || strUnSelect_ == "" || devName_ == "") {
        LOG(CENCTRL_ERROR, "onclick param err\n");
        return false;
    }

    char sendbuf[TMP_BUF_MAX] = {0};
    if (currState_ == false) {
        sprintf_s(sendbuf, TMP_BUF_MAX - 1, "HMTAON");
        if (WriteSocket(devName_.c_str(), sendbuf, strlen(sendbuf)) == 0) {
            label_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
            image_->SetSrc(strSelect_.c_str());
            currState_ = true;
        }
    } else {
        sprintf_s(sendbuf, TMP_BUF_MAX - 1, "HMTAOFF");
        if (WriteSocket(devName_.c_str(), sendbuf, strlen(sendbuf)) == 0) {
            label_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
            image_->SetSrc(strUnSelect_.c_str());
            currState_ = false;
        }
    }
    view_->Invalidate();
    return true;
}

void ItemCtrl::OnConnect(char* devName, int status)
{
    LOG(CENCTRL_DEBUG, "Item connect \n");
    view_->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY_CHANGE);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    view_->Invalidate();
}

void ItemCtrl::OnDisconnect(char* devName, int status)
{
    LOG(CENCTRL_DEBUG, "Item disconnect \n");
    view_->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Gray()));
    currState_ = false;
    view_->Invalidate();
}

void ItemCtrl::On(char* devName, int status)
{
    LOG(CENCTRL_DEBUG, "Item on \n");
    view_->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
    view_->Invalidate();
}

void ItemCtrl::Off(char* devName, int status)
{
    LOG(CENCTRL_DEBUG, "Item off \n");
    view_->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Gray()));
    currState_ = false;
    view_->Invalidate();
}
}